/*************************************************************************//**
 * \file    SHMeshCompiler.cpp
 * \author  Loh Xiao Qi
 * \date    30 September 2022
 * \brief   Library to write data in SHMeshAsset into binary file for faster
 *					loading in the future
 * 
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/
#include "SHMeshCompiler.h"
#include <assimp/postprocess.h>

#include <fstream>
#include <iostream>

namespace SHADE
{

  Assimp::Importer SHMeshCompiler::aiImporter;

  void SHMeshCompiler::ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes) noexcept
  {
    for (size_t i{ 0 }; i < node.mNumMeshes; ++i)
    {
      aiMesh* mesh = scene.mMeshes[node.mMeshes[i]];
      meshes.push_back(ProcessMesh(*mesh));
    }

    for (size_t i{ 0 }; i < node.mNumChildren; ++i)
    {
      ProcessNode(*node.mChildren[i], scene, meshes);
    }
  }

  void SHMeshCompiler::ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept
  {
    if (scene.HasAnimations())
    {
      std::vector<SHAnimationAsset> anims(scene.mNumAnimations);
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

  SHMeshAsset* SHMeshCompiler::ProcessMesh(aiMesh const& mesh) noexcept
  {
    SHMeshAsset* result = new SHMeshAsset();
    result->compiled = false;
    result->changed = false;

    for (size_t i{ 0 }; i < mesh.mNumVertices; ++i)
    {
      // Vertex position
      SHVec3 vertex;
      vertex.x = mesh.mVertices[i].x;
      vertex.y = mesh.mVertices[i].y;
      vertex.z = mesh.mVertices[i].z;
      result->vertexPosition.push_back(vertex);

      // Tex coords
      SHVec2 texCoord{ 0.f, 0.f };
      if (mesh.mTextureCoords[0])
      {
        texCoord.x = mesh.mTextureCoords[0][i].x;
        texCoord.y = mesh.mTextureCoords[0][i].y;
      }
      result->texCoords.push_back(texCoord);

      // Normals
      SHVec3 normal{ 0.f, 0.f, 0.f };
      if (mesh.mNormals)
      {
        normal.x = mesh.mNormals[i].x;
        normal.y = mesh.mNormals[i].y;
        normal.z = mesh.mNormals[i].z;
      }
      result->vertexNormal.push_back(normal);

      // Tangent
      SHVec3 tangent{ 0.f, 0.f, 0.f };
      if (mesh.mTangents)
      {
        tangent.x = mesh.mTangents[i].x;
        tangent.y = mesh.mTangents[i].y;
        tangent.z = mesh.mTangents[i].z;
      }
      result->vertexTangent.push_back(tangent);
    }

    for (size_t i{ 0 }; i < mesh.mNumFaces; ++i)
    {
      aiFace face = mesh.mFaces[i];
      for (size_t j{ 0 }; j < face.mNumIndices; ++j)
      {
        result->indices.push_back(face.mIndices[j]);
      }
    }

    result->header.vertexCount = static_cast<uint32_t>(result->vertexPosition.size());
    result->header.indexCount = static_cast<uint32_t>(result->indices.size());
    result->header.name = mesh.mName.C_Str();

    return result;
  }

  void SHMeshCompiler::LoadFromFile(AssetPath path, MeshVectorRef meshes, AnimVectorRef anims) noexcept
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

    ProcessNode(*scene->mRootNode, *scene, meshes);

    aiImporter.FreeScene();
  }

  std::optional<AssetPath> SHMeshCompiler::CompileMeshBinary(SHMeshAsset const& asset, AssetPath path) noexcept
  {
    std::string newPath{ path.string() + asset.header.name + MESH_EXTENSION.data() };

    std::ofstream file{ newPath, std::ios::out | std::ios::binary | std::ios::trunc };
    if (!file.is_open())
    {
      std::cout << "Unable to open file for write: " << newPath << std::endl;
      return {};
    }

    file.write(
      reinterpret_cast<char const*>(&(asset.header.vertexCount)),
      sizeof(uint32_t)
    );

    file.write(
      reinterpret_cast<const char*>(&(asset.header.indexCount)),
      sizeof(uint32_t)
    );

    auto const vertexVec3Byte{ sizeof(SHVec3) * asset.header.vertexCount };
    auto const vertexVec2Byte{ sizeof(SHVec2) * asset.header.vertexCount };

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
      sizeof(uint32_t) * asset.header.indexCount
    );

    file.close();

    return newPath;
  }

  void SHMeshCompiler::LoadAndCompile(AssetPath path) noexcept
  {
    std::vector<SHMeshAsset*> meshes;
    std::vector<SHAnimationAsset*> anims;

    LoadFromFile(path, meshes, anims);

    for (auto const& mesh : meshes)
    {
      CompileMeshBinary(*mesh, path.parent_path().string() + '/');
    }
  }
}
