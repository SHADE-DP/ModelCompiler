/*************************************************************************//**
 * \file    MeshCompiler.cpp
 * \author  Loh Xiao Qi
 * \date    30 September 2022
 * \brief   Library to write data in MeshAsset into binary file for faster
 *					loading in the future
 * 
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/
#include "MeshCompiler.h"
#include <assimp/postprocess.h>

#include <fstream>
#include <iostream>

#include <queue>

namespace SH_COMP
{

  Assimp::Importer MeshCompiler::aiImporter;
  uint32_t MeshCompiler::rigNodeIDCounter { 0 };

  void MeshCompiler::ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes, RigData& rig) noexcept
  {
    for (size_t i{ 0 }; i < node.mNumMeshes; ++i)
    {
      aiMesh* mesh = scene.mMeshes[node.mMeshes[i]];
      meshes.emplace_back();
      GetMesh(*mesh, meshes.back());
      meshes.back().name = node.mName.C_Str();
    }

    if (std::strcmp(node.mName.C_Str(), "Armature") == 0)
    {
      BuildArmature(node, rig.root);
    }
    else
    {
      for (size_t i{ 0 }; i < node.mNumChildren; ++i)
      {
        ProcessNode(*node.mChildren[i], scene, meshes, rig);
      }
    }
  }

  void MeshCompiler::GetMesh(aiMesh const& mesh, MeshData& meshData) noexcept
  {
    meshData.vertexPosition.reserve(mesh.mNumVertices);
    meshData.vertexNormal.reserve(mesh.mNumVertices);
    meshData.vertexTangent.reserve(mesh.mNumVertices);
    meshData.texCoords.reserve(mesh.mNumVertices);

    meshData.bonesInfo.resize(mesh.mNumBones);
    meshData.bones.resize(mesh.mNumBones);

    for (auto i {0}; i < mesh.mNumBones; ++i)
    {
      auto const& bone = *mesh.mBones[i];
      auto& newBone = meshData.bones[i];
      auto& newBoneInfo = meshData.bonesInfo[i];

      newBone.name = bone.mName.C_Str();
      newBoneInfo.charCount = newBone.name.length();

      std::memcpy(&newBone.offset, &bone.mOffsetMatrix, sizeof(SHMat4));

      newBone.weights.resize(bone.mNumWeights);
      for (auto j {0}; j < bone.mNumWeights; ++j)
      {
        newBone.weights[j].index = bone.mWeights[j].mVertexId;
        newBone.weights[j].weight = bone.mWeights[j].mWeight;
      }
      newBoneInfo.weightCount = bone.mNumWeights;
    }

    for (size_t i{ 0 }; i < mesh.mNumVertices; ++i)
    {
      // Vertex position
      SHVec3 vertex;
      vertex.x = mesh.mVertices[i].x;
      vertex.y = mesh.mVertices[i].y;
      vertex.z = mesh.mVertices[i].z;
      meshData.vertexPosition.push_back(vertex);

      // Tex coords
      SHVec2 texCoord{ 0.f, 0.f };
      if (mesh.mTextureCoords[0])
      {
        texCoord.x = mesh.mTextureCoords[0][i].x;
        texCoord.y = mesh.mTextureCoords[0][i].y;
      }
      meshData.texCoords.push_back(texCoord);

      // Normals
      SHVec3 normal{ 0.f, 0.f, 0.f };
      if (mesh.mNormals)
      {
        normal.x = mesh.mNormals[i].x;
        normal.y = mesh.mNormals[i].y;
        normal.z = mesh.mNormals[i].z;
      }
      meshData.vertexNormal.push_back(normal);

      // Tangent
      SHVec3 tangent{ 0.f, 0.f, 0.f };
      if (mesh.mTangents)
      {
        tangent.x = mesh.mTangents[i].x;
        tangent.y = mesh.mTangents[i].y;
        tangent.z = mesh.mTangents[i].z;
      }
      meshData.vertexTangent.push_back(tangent);
    }

    for (size_t i{ 0 }; i < mesh.mNumFaces; ++i)
    {
      aiFace face = mesh.mFaces[i];
      for (size_t j{ 0 }; j < face.mNumIndices; ++j)
      {
        meshData.indices.push_back(face.mIndices[j]);
      }
    }
  }

  void MeshCompiler::BuildHeaders(ModelAsset& asset) noexcept
  {
    // Mesh Headers
    asset.meshHeaders.resize(asset.meshes.size());
    asset.header.meshCount = asset.meshes.size();
    for (auto i{0}; i < asset.header.meshCount; ++i)
    {
      auto const& mesh = asset.meshes[i];
      auto& head = asset.meshHeaders[i];

      head.charCount = mesh.name.size();
      head.indexCount = mesh.indices.size();
      head.vertexCount = mesh.vertexPosition.size();
      head.boneCount = mesh.bonesInfo.size();
    }

    // Anim Headers
    asset.animHeaders.resize(asset.anims.size());
    asset.header.animCount = asset.anims.size();
    for (auto i{0}; i < asset.header.animCount; ++i)
    {
      auto const& anim = asset.anims[i];
      auto& head = asset.animHeaders[i];
      
      head.charCount = anim.name.size();
      head.animNodeCount = anim.nodeChannels.size();
      head.nodeHeaders.resize(head.animNodeCount);

      for (auto j{0}; j < head.animNodeCount; ++j)
      {
	      auto const& animNode = anim.nodeChannels[j];
        auto& nodeHeader = head.nodeHeaders[j];

        nodeHeader.charCount = animNode.name.size();
        nodeHeader.posKeyCount = animNode.positionKeys.size();
        nodeHeader.rotKeyCount = animNode.rotationKeys.size();
        nodeHeader.scaKeyCount = animNode.scaleKeys.size();
      }
    }
  }

  void MeshCompiler::WriteMeshHeader(std::ofstream& file, MeshDataHeader const& header)
  {
    file.write(
      reinterpret_cast<char const*>(&header),
      sizeof(MeshDataHeader)
    );
  }

  void MeshCompiler::WriteMeshData(std::ofstream& file, MeshDataHeader const& header, MeshData const& asset)
  {
    auto const vertexVec3Byte{ sizeof(SHVec3) * header.vertexCount };
    auto const vertexVec2Byte{ sizeof(SHVec2) * header.vertexCount };

    file.write(
      asset.name.c_str(),
      header.charCount
    );

    file.write(
      reinterpret_cast<char const*>(asset.vertexPosition.data()),
      vertexVec3Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.vertexTangent.data()),
      vertexVec3Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.vertexNormal.data()),
      vertexVec3Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.texCoords.data()),
      vertexVec2Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.indices.data()),
      sizeof(uint32_t) * header.indexCount
    );
  }

  void MeshCompiler::WriteAnimHeader(FileReference file, AnimDataHeader const& header)
  {
    auto constexpr intSize = sizeof(uint32_t);

    file.write(
      reinterpret_cast<char const*>(&header.charCount),
      intSize
    );

    file.write(
      reinterpret_cast<char const*>(&header.animNodeCount),
      intSize
    );
    
    file.write(
      reinterpret_cast<char const*>(header.nodeHeaders.data()),
      sizeof(AnimNodeInfo) * header.nodeHeaders.size()
    );
  }

  void MeshCompiler::WriteAnimData(FileReference file, AnimDataHeader const& header, AnimData const& data)
  {
    file.write(
      data.name.data(),
      header.charCount
    );

    file.write(
      reinterpret_cast<char const*>(&data.duration),
      sizeof(double)
    );

    file.write(
      reinterpret_cast<char const*>(&data.ticksPerSecond),
      sizeof(double)
    );

		for (auto i{0}; i < header.animNodeCount; ++i)
		{
			WriteAnimNode(file, header.nodeHeaders[i], data.nodeChannels[i]);
		}
  }

  void MeshCompiler::WriteAnimNode(FileReference file, AnimNodeInfo const& info, AnimNode const& node)
  {
    file.write(
      node.name.data(),
      info.charCount
    );

    file.write(
      reinterpret_cast<char const*>(&node.pre),
      sizeof(AnimationBehaviour)
    );

    file.write(
      reinterpret_cast<char const*>(&node.post),
      sizeof(AnimationBehaviour)
    );

    file.write(
      reinterpret_cast<char const*>(node.positionKeys.data()),
      sizeof(PositionKey) * node.positionKeys.size()
    );

    file.write(
      reinterpret_cast<char const*>(node.rotationKeys.data()),
      sizeof(RotationKey) * node.rotationKeys.size()
    );

    file.write(
      reinterpret_cast<char const*>(node.scaleKeys.data()),
      sizeof(ScaleKey) * node.scaleKeys.size()
    );
  }

  void MeshCompiler::WriteHeaders(FileReference file, ModelConstRef asset)
  {
    for (auto const& header : asset.meshHeaders)
    {
	    WriteMeshHeader(file, header);
    }

    for (auto  const& header : asset.animHeaders)
    {
	    WriteAnimHeader(file, header);
    }
  }

  void MeshCompiler::WriteData(FileReference file, ModelConstRef asset)
  {
    for (auto i {0}; i < asset.meshes.size(); ++i)
    {
	    WriteMeshData(file, asset.meshHeaders[i], asset.meshes[i]);
    }
    
    for (auto i {0}; i < asset.anims.size(); ++i)
    {
	    WriteAnimData(file, asset.animHeaders[i], asset.anims[i]);
    }
  }

  void MeshCompiler::ParseAnimations(aiScene const& scene, std::vector<AnimData>& anims) noexcept
  {
    // Size and read for number of animation clips
    anims.resize(scene.mNumAnimations);
    for (auto i {0}; i < scene.mNumAnimations; ++i)
    {
	    auto const& animData = *scene.mAnimations[i];
      auto& anim = anims[i];

      anim.name = animData.mName.C_Str();
      anim.duration = animData.mDuration;
      anim.ticksPerSecond = animData.mTicksPerSecond;

      // Size and read for number of animation frames
      anim.nodeChannels.resize(animData.mNumChannels);
      for (auto j {0}; j < animData.mNumChannels; ++j)
      {
        auto const& channelData = *animData.mChannels[j];
        auto& node = anim.nodeChannels[j];

        node.name = channelData.mNodeName.C_Str();

        // Position Keys
        node.positionKeys.resize(channelData.mNumPositionKeys);
        for (auto k{0}; k < channelData.mNumPositionKeys; ++k)
        {
	        auto const& posKeyData = channelData.mPositionKeys[k];
          auto& posKey = node.positionKeys[k];

          posKey.time = posKeyData.mTime;
          posKey.value.x = posKeyData.mValue.x;
          posKey.value.y = posKeyData.mValue.y;
          posKey.value.z = posKeyData.mValue.z;
        }
        
        // Rotation Keys
        node.rotationKeys.resize(channelData.mNumRotationKeys);
        for (auto k{0}; k < channelData.mNumRotationKeys; ++k)
        {
	        auto const& posKeyData = channelData.mRotationKeys[k];
          auto& posKey = node.rotationKeys[k];

          posKey.time = posKeyData.mTime;
          posKey.value.x = posKeyData.mValue.x;
          posKey.value.y = posKeyData.mValue.y;
          posKey.value.z = posKeyData.mValue.z;
          posKey.value.w = posKeyData.mValue.w;
        }
        
        // Scale Keys
        node.scaleKeys.resize(channelData.mNumScalingKeys);
        for (auto k{0}; k < channelData.mNumScalingKeys; ++k)
        {
	        auto const& posKeyData = channelData.mScalingKeys[k];
          auto& posKey = node.scaleKeys[k];

          posKey.time = posKeyData.mTime;
          posKey.value.x = posKeyData.mValue.x;
          posKey.value.y = posKeyData.mValue.y;
          posKey.value.z = posKeyData.mValue.z;
        }
      }
    }
  }

  void MeshCompiler::LoadFromFile(AssetPath path, ModelAsset& asset) noexcept
  {
    const aiScene* scene = aiImporter.ReadFile(path.string().c_str(),
      aiProcess_Triangulate                 // Make sure we get triangles rather than nvert polygons
      | aiProcess_GenUVCoords               // Convert any type of mapping to uv mapping
      | aiProcess_TransformUVCoords         // preprocess UV transformations (scaling, translation ...)
      | aiProcess_FindInstances             // search for instanced meshes and remove them by references to one master
      | aiProcess_CalcTangentSpace          // calculate tangents and bitangents if possible
      | aiProcess_JoinIdenticalVertices     // join identical vertices/ optimize indexing
      | aiProcess_FindInvalidData           // detect invalid model data, such as invalid normal vectors
      | aiProcess_FlipUVs                   // flip the V to match the Vulkans way of doing UVs
      | aiProcess_ValidateDataStructure
    );

    if (!scene || !scene->HasMeshes())
    {
      std::cout << "ERROR in GLTF::ASSIMP: " << aiImporter.GetErrorString() << "\nFile: " << path.string() << std::endl;
      return;
    }

    ParseAnimations(*scene, asset.anims);

    ProcessNode(*scene->mRootNode, *scene, asset.meshes, asset.rig);

    aiImporter.FreeScene();
  }

  void MeshCompiler::CompileMeshBinary(AssetPath path, ModelAsset const& asset) noexcept
  {
    std::string newPath{ path.string().substr(0, path.string().find_last_of('.')) };
    newPath += MODEL_EXTENSION;

    std::ofstream file{ newPath, std::ios::out | std::ios::binary | std::ios::trunc };
    if (!file.is_open())
    {
      std::cout << "Unable to open file for write: " << newPath << std::endl;
      return;
    }

    file.write(
      reinterpret_cast<char const*>(&asset.header),
      sizeof(asset.header)
    );

    WriteHeaders(file, asset);
    WriteData(file, asset);

    file.close();
  }

  void MeshCompiler::BuildArmature(aiNode const& baseNode, RigData& rig) noexcept
  {
    std::queue<aiNode const*> nodesQueue;
    nodesQueue.push(&baseNode);

    RigNode* parent = nullptr;

    while(!nodesQueue.empty())
    {

    }
  }

  void MeshCompiler::LoadAndCompile(AssetPath path) noexcept
  {
    auto const asset = new ModelAsset();

    LoadFromFile(path, *asset);
    BuildHeaders(*asset);
    CompileMeshBinary(path, *asset);

    delete asset;
  }
}
