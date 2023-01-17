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
#include "MeshWriter.h"
#include <assimp/postprocess.h>

#include <fstream>
#include <iostream>

#include <stack>

namespace SH_COMP
{

  SHMat4 aiTransformToMat4(aiMatrix4x4 const& inMatrx)
  {
	  SHMat4 result;

    std::memcpy(
      &result,
      &inMatrx,
      sizeof(result)
    );

    return result;
  }

  Assimp::Importer MeshCompiler::aiImporter;
  uint32_t MeshCompiler::rigNodeIDCounter { 0 };

  void MeshCompiler::ProcessNode(AiNodeConstPtr node, aiScene const& scene, MeshVectorRef meshes, RigData& rig) noexcept
  {
    if (node->mNumMeshes > 0)
    {
      aiMesh* mesh = scene.mMeshes[node->mMeshes[0]];
      meshes.emplace_back();
      GetMesh(*mesh, meshes.back());
      meshes.back().name = node->mName.C_Str();
    }
    else if (node->mParent != nullptr)
    {
      BuildArmature(node, rig);
      return;
    }

    for (auto i{ 0 }; i < node->mNumChildren; ++i)
    {
      ProcessNode(node->mChildren[i], scene, meshes, rig);
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

    for (auto i{ 0 }; i < mesh.mNumBones; ++i)
    {
      auto const& bone = *mesh.mBones[i];
      auto& newBone = meshData.bones[i];
      auto& newBoneInfo = meshData.bonesInfo[i];

      newBone.name = bone.mName.C_Str();
      newBoneInfo.charCount = newBone.name.length();

      std::memcpy(&newBone.offset, &bone.mOffsetMatrix, sizeof(SHMat4));

      newBone.weights.resize(bone.mNumWeights);
      for (auto j{ 0 }; j < bone.mNumWeights; ++j)
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

  void MeshCompiler::BuildHeaders(ModelRef asset) noexcept
  {
    // Mesh Headers
    asset.meshHeaders.resize(asset.meshes.size());
    asset.header.meshCount = asset.meshes.size();
    for (auto i{ 0 }; i < asset.header.meshCount; ++i)
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
    for (auto i{ 0 }; i < asset.header.animCount; ++i)
    {
      auto const& anim = asset.anims[i];
      auto& head = asset.animHeaders[i];

      head.charCount = anim.name.size();
      head.animNodeCount = anim.nodeChannels.size();
      head.nodeHeaders.resize(head.animNodeCount);

      for (auto j{ 0 }; j < head.animNodeCount; ++j)
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

  void MeshCompiler::ParseAnimations(aiScene const& scene, std::vector<AnimData>& anims) noexcept
  {
    // Size and read for number of animation clips
    anims.resize(scene.mNumAnimations);
    for (auto i{ 0 }; i < scene.mNumAnimations; ++i)
    {
      auto const& animData = *scene.mAnimations[i];
      auto& anim = anims[i];

      anim.name = animData.mName.C_Str();
      anim.duration = animData.mDuration;
      anim.ticksPerSecond = animData.mTicksPerSecond;

      // Size and read for number of animation frames
      anim.nodeChannels.resize(animData.mNumChannels);
      for (auto j{ 0 }; j < animData.mNumChannels; ++j)
      {
        auto const& channelData = *animData.mChannels[j];
        auto& node = anim.nodeChannels[j];

        node.name = channelData.mNodeName.C_Str();

        // Position Keys
        node.positionKeys.resize(channelData.mNumPositionKeys);
        for (auto k{ 0 }; k < channelData.mNumPositionKeys; ++k)
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
        for (auto k{ 0 }; k < channelData.mNumRotationKeys; ++k)
        {
          auto const& rotKeyData = channelData.mRotationKeys[k];
          auto& rotKey = node.rotationKeys[k];

          rotKey.time = rotKeyData.mTime;
          rotKey.value.x = rotKeyData.mValue.x;
          rotKey.value.y = rotKeyData.mValue.y;
          rotKey.value.z = rotKeyData.mValue.z;
          rotKey.value.w = rotKeyData.mValue.w;
        }

        // Scale Keys
        node.scaleKeys.resize(channelData.mNumScalingKeys);
        for (auto k{ 0 }; k < channelData.mNumScalingKeys; ++k)
        {
          auto const& scaKeyData = channelData.mScalingKeys[k];
          auto& scaKey = node.scaleKeys[k];

          scaKey.time = scaKeyData.mTime;
          scaKey.value.x = scaKeyData.mValue.x;
          scaKey.value.y = scaKeyData.mValue.y;
          scaKey.value.z = scaKeyData.mValue.z;
        }
      }
    }
  }

  std::pair<RigNodeData*, aiNode const*> MeshCompiler::PairHelper(AiNodeConstPtr node)
  {
    return std::make_pair(NewNode(node), node);
  }

  RigNodeData* MeshCompiler::NewNode(AiNodeConstPtr inNode)
  {
    return new RigNodeData(inNode->mName.C_Str(), aiTransformToMat4(inNode->mTransformation));
  }

  void MeshCompiler::LoadFromFile(AssetPath path, ModelRef asset) noexcept
  {
    const aiScene* scene = aiImporter.ReadFile(path.string().c_str(), 
      aiProcess_Triangulate                 // Make sure we get triangles rather than nvert polygons
      | aiProcess_GenUVCoords               // Convert any type of mapping to uv mapping
      | aiProcess_TransformUVCoords         // preprocess UV transformations (scaling, translation ...)
      | aiProcess_FindInstances             // search for instanced meshes and remove them by references to one master
      | aiProcess_CalcTangentSpace          // calculate tangents and bitangents if possible
      | aiProcess_JoinIdenticalVertices     // join identical vertices/ optimize indexing
      | aiProcess_FindInvalidData           // detect invalid model data, such as invalid normal vector
      | aiProcess_FlipUVs                   // flip the V to match the Vulkans way of doing UVs
      | aiProcess_ValidateDataStructure     // checks all bones, animations and vertices are linked correctly
      //| aiProcess_LimitBoneWeights          // Limit number of bones effect vertices to 4
    );

    if (!scene || !scene->HasMeshes())
    {
      std::cout << "ERROR in GLTF::ASSIMP: " << aiImporter.GetErrorString() << "\nFile: " << path.string() << std::endl;
      return;
    }

    ProcessNode(scene->mRootNode, *scene, asset.meshes, asset.rig);

    ParseAnimations(*scene, asset.anims);

    aiImporter.FreeScene();
  }

  void MeshCompiler::BuildArmature(AiNodeConstPtr baseNode, RigData& rig) noexcept
  {
    // Build implementation copy of armature tree
    // node collection write done later when writing to file
    std::stack<std::pair<RigNodeData*, AiNodeConstPtr>> nodeStack;

    nodeStack.emplace(PairHelper(baseNode));
    rig.root = nodeStack.top().first;

    while(!nodeStack.empty())
    {
      auto currPair = nodeStack.top();
      nodeStack.pop();
      auto currNode = currPair.first;
      auto const& currAiNode = currPair.second;
      int const iStart {static_cast<int>(currAiNode->mNumChildren - 1)};

      rig.header.nodeCount++;
      rig.header.charCounts.push_back(currNode->name.length());

      for (int i {iStart}; i >= 0 ; --i)
      {
        auto newPair = PairHelper(currAiNode->mChildren[i]);
        currNode->children.push_back(newPair.first);
        nodeStack.push(newPair);
      }
    }
  }

  void MeshCompiler::LoadAndCompile(AssetPath path) noexcept
  {
    auto const asset = new ModelAsset();

    LoadFromFile(path, *asset);
    BuildHeaders(*asset);
    MeshWriter::CompileMeshBinary(path, *asset);

    delete asset;
  }
}
