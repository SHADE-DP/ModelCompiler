/*************************************************************************//**
 * \file    SHAssimpLibrary.cpp
 * \author  Loh Xiao Qi
 * \date    October 2022
 * \brief    
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/
#include "SHAssimpLibrary.h"
#include <assimp/postprocess.h>
#include <iostream>
#include <algorithm>

namespace SHADE
{
	Assimp::Importer SHAssimpLibrary::aiImporter;

	void SHAssimpLibrary::ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes) noexcept
	{
    for (size_t i {0}; i < node.mNumMeshes; ++i)
    {
      aiMesh* mesh = scene.mMeshes[node.mMeshes[i]];
      meshes.emplace_back();
      ProcessMesh(*mesh, meshes.back());
    }

    for (size_t i{ 0 }; i < node.mNumChildren; ++i)
    {
      ProcessNode(*node.mChildren[i], scene, meshes);
    }
	}

	void SHAssimpLibrary::ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept
	{
		if (scene.HasAnimations())
    {
			std::vector<SHAnimationAsset> anims(scene.mNumAnimations);
	    for (auto i{0}; i < scene.mNumAnimations; ++i)
	    {
		    auto const& anim {*scene.mAnimations[i]};

        anims[i].name = anim.mName.C_Str();

        anims[i].duration = anim.mDuration;
        anims[i].ticksPerSecond = anim.mTicksPerSecond;

        std::copy_n(anim.mChannels, anim.mNumChannels, anims[i].nodeChannels.data());
        std::copy_n(anim.mMeshChannels, anim.mNumMeshChannels, anims[i].meshChannels.data());
        std::copy_n(anim.mMorphMeshChannels, anim.mNumMorphMeshChannels, anims[i].morphMeshChannels.data());
	    }
    }
	}

	void SHAssimpLibrary::ProcessMesh(aiMesh const& mesh, SHMeshAsset& meshDest) noexcept
	{
      meshDest.compiled = false;
      meshDest.changed = false;

    for (size_t i{0}; i < mesh.mNumVertices; ++i)
    {
      // Vertex position
      SHVec3 vertex;
      vertex.x = mesh.mVertices[i].x;
      vertex.y = mesh.mVertices[i].y;
      vertex.z = mesh.mVertices[i].z;
      meshDest.vertexPosition.push_back(vertex);

      // Tex coords
      SHVec2 texCoord{0.f, 0.f};
      if (mesh.mTextureCoords[0])
      {
        texCoord.x = mesh.mTextureCoords[0][i].x;
        texCoord.y = mesh.mTextureCoords[0][i].y;
      }
      meshDest.texCoords.push_back(texCoord);

      // Normals
      SHVec3 normal{0.f, 0.f, 0.f};
      if (mesh.mNormals)
      {
        normal.x = mesh.mNormals[i].x;
        normal.y = mesh.mNormals[i].y;
        normal.z = mesh.mNormals[i].z;
      }
      meshDest.vertexNormal.push_back(normal);

      // Tangent
      SHVec3 tangent{0.f, 0.f, 0.f};
      if (mesh.mTangents)
      {
        tangent.x = mesh.mTangents[i].x;
        tangent.y = mesh.mTangents[i].y;
        tangent.z = mesh.mTangents[i].z;
      }
      meshDest.vertexTangent.push_back(tangent);
    }

    for (size_t i {0}; i < mesh.mNumFaces; ++i)
    {
      aiFace face = mesh.mFaces[i];
      for (size_t j{0}; j < face.mNumIndices; ++j)
      {
        meshDest.indices.push_back(face.mIndices[j]);
      }
    }

    meshDest.header.vertexCount = static_cast<uint32_t>(meshDest.vertexPosition.size());
    meshDest.header.indexCount = static_cast<uint32_t>(meshDest.indices.size());
    meshDest.header.meshName = mesh.mName.C_Str();
	}

	void SHAssimpLibrary::LoadFromFile(AssetPath path, MeshVectorRef meshes, AnimVectorRef anims) noexcept
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
      std::cout << "ERROR in GLTF::ASSIMP: " <<  aiImporter.GetErrorString() << "\nFile: " << path.string() << std::endl;
      return;
    }

    ExtractAnimations(*scene, anims);

    MeshVector localMesh(scene->mNumMeshes);
    ProcessNode(*scene->mRootNode, *scene, localMesh);

    for (auto& mesh : localMesh)
    {
	    mesh.parentPath = path.parent_path().string();
    }

    std::ranges::move(localMesh.begin(), localMesh.end(), meshes.end());

    aiImporter.FreeScene();
	}
}
