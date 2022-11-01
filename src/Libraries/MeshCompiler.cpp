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

namespace SH_COMP
{

  Assimp::Importer MeshCompiler::aiImporter;

  void MeshCompiler::ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes) noexcept
  {
    for (size_t i{ 0 }; i < node.mNumMeshes; ++i)
    {
      aiMesh* mesh = scene.mMeshes[node.mMeshes[i]];
      meshes.emplace_back();
      GetMesh(*mesh, meshes.back());
    }

    for (size_t i{ 0 }; i < node.mNumChildren; ++i)
    {
      ProcessNode(*node.mChildren[i], scene, meshes);
    }
  }

  void MeshCompiler::ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept
  {
    if (scene.HasAnimations())
    {
      std::vector<AnimationAsset> anims(scene.mNumAnimations);
      for (auto i{ 0 }; i < scene.mNumAnimations; ++i)
      {
        auto const& anim{ *scene.mAnimations[i] };

        anims[i].name = anim.mName.C_Str();

        anims[i].duration = anim.mDuration;
        anims[i].ticksPerSecond = anim.mTicksPerSecond;

        std::copy_n(anim.mChannels, anim.mNumChannels, anims[i].nodeChannels.data());
        std::copy_n(anim.mMeshChannels, anim.mNumMeshChannels, anims[i].meshChannels.data());
        std::copy_n(anim.mMorphMeshChannels, anim.mNumMorphMeshChannels, anims[i].morphMeshChannels.data());
      }
    }
  }

  void MeshCompiler::GetMesh(aiMesh const& mesh, MeshData& meshData) noexcept
  {
    meshData.vertexPosition.reserve(mesh.mNumVertices);
    meshData.vertexNormal.reserve(mesh.mNumVertices);
    meshData.vertexTangent.reserve(mesh.mNumVertices);
    meshData.texCoords.reserve(mesh.mNumVertices);

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

    meshData.name = mesh.mName.C_Str();
  }

  void MeshCompiler::BuildHeaders(MeshAsset& asset) noexcept
  {
    for (auto const& mesh : asset.meshes)
    {
      asset.headers.emplace_back();
      auto& head = asset.headers.back();

      head.charCount = mesh.name.size();
      head.indexCount = mesh.indices.size();
      head.vertexCount = mesh.vertexPosition.size();

      asset.header.meshCount++;
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

  void MeshCompiler::LoadFromFile(AssetPath path, MeshAsset& asset) noexcept
  {
    const aiScene* scene = aiImporter.ReadFile(path.string().c_str(),
      aiProcess_Triangulate                 // Make sure we get triangles rather than nvert polygons
      | aiProcess_GenUVCoords               // Convert any type of mapping to uv mapping
      | aiProcess_TransformUVCoords         // preprocess UV transformations (scaling, translation ...)
      | aiProcess_FindInstances             // search for instanced meshes and remove them by references to one master
      | aiProcess_CalcTangentSpace          // calculate tangents and bitangents if possible
      | aiProcess_JoinIdenticalVertices     // join identical vertices/ optimize indexing
      | aiProcess_RemoveRedundantMaterials  // remove redundant materials
      | aiProcess_FindInvalidData           // detect invalid model data, such as invalid normal vectors
      | aiProcess_FlipUVs                   // flip the V to match the Vulkans way of doing UVs
    );

    if (!scene || !scene->HasMeshes())
    {
      std::cout << "ERROR in GLTF::ASSIMP: " << aiImporter.GetErrorString() << "\nFile: " << path.string() << std::endl;
      return;
    }

    //ExtractAnimations(*scene, anims);

    ProcessNode(*scene->mRootNode, *scene, asset.meshes);

    aiImporter.FreeScene();
  }

  void MeshCompiler::CompileMeshBinary(AssetPath path, MeshAsset const& asset) noexcept
  {
    std::string newPath{ path.string().substr(0, path.string().find_last_of('.')) };
    newPath += MESH_EXTENSION;

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

    for (auto i {0}; i < asset.headers.size(); ++i)
    {
      WriteMeshHeader(file, asset.headers[i]);
      WriteMeshData(file, asset.headers[i], asset.meshes[i]);
    }

    file.close();
  }

  void MeshCompiler::LoadAndCompile(AssetPath path) noexcept
  {
    auto const asset = new MeshAsset();

    LoadFromFile(path, *asset);
    BuildHeaders(*asset);
    CompileMeshBinary(path, *asset);

    delete asset;
  }
}
