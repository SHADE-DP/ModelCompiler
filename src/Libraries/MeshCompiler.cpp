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


#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE

#include "MeshCompiler.h"
#include "MeshWriter.h"

#include <fstream>
#include <iostream>

#include "tiny_gltf.h"
#include <map>
#include <stack>

namespace SH_COMP
{
  AccessorReference MeshCompiler::accessors{ nullptr };
  BufferViewReference MeshCompiler::bufferViews{ nullptr };
  BufferData MeshCompiler::buffer{ nullptr };

  void MeshCompiler::LoadFromFile(AssetPath path, ModelRef asset) noexcept
  {
    ModelData model;
    tinygltf::TinyGLTF loader;
    std::string warn, error;

    bool result = loader.LoadASCIIFromFile(&model, &error, &warn, path.string());

    if (!warn.empty())
      std::cout << "[TinyGLTF Warning]: " << warn << std::endl;

    if (!error.empty())
      std::cout << "[TinyGLTF Error]: " << error << std::endl;

    if (!result)
    { 
	    std::cout << "TinyGLTF failed to parse.\n";
      std::exit(1);
    }

    ProcessModel(model, asset);
  }

  void MeshCompiler::ProcessModel(ModelData const& data, ModelRef asset) noexcept
  {
#if 0
    for (auto i {0}; i < 2; ++i)
    {
      auto& mesh{ asset.meshes.emplace_back() };
      mesh.vertexNormal.resize(3883);
      mesh.vertexPosition.resize(3883);
      mesh.vertexTangent.resize(3883);
      mesh.texCoords.resize(3883);
    }
#else
    accessors = &data.accessors;
    bufferViews = &data.bufferViews;
    buffer = data.buffers[0].data.data();

    for (auto const& mesh : data.meshes)
    {
      auto const& primitive { mesh.primitives[0] };
      auto& meshIn {asset.meshes.emplace_back()};
      meshIn.name = mesh.name;

      try
      {
        //meshIn.vertexPosition = FetchData<SHVec3>(primitive.attributes.at(ATT_POSITION.data()));
        //meshIn.vertexNormal = FetchData<SHVec3>(primitive.attributes.at(ATT_NORMAL.data()));
        //meshIn.vertexTangent = FetchData<SHVec3>(primitive.attributes.at(ATT_TANGENT.data()));
        //meshIn.texCoords = FetchData<SHVec2>(primitive.attributes.at(ATT_TEXCOORD.data()));

        auto accessor = &(*accessors)[primitive.attributes.at(ATT_POSITION.data())];
        auto view = &(*bufferViews)[accessor->bufferView];
        meshIn.vertexPosition.resize(accessor->count);
        std::memcpy(
          meshIn.vertexPosition.data(),
          buffer + view->byteOffset,
          view->byteLength
        );

        accessor = &(*accessors)[primitive.indices];
        view = &(*bufferViews)[accessor->bufferView];
        meshIn.indices.resize(accessor->count);
        std::memcpy(
          meshIn.indices.data(),
          buffer + view->byteOffset,
          view->byteLength
        );

        accessor = &(*accessors)[primitive.attributes.at(ATT_NORMAL.data())];
        view = &(*bufferViews)[accessor->bufferView];
        meshIn.vertexNormal.resize(accessor->count);
        std::memcpy(
          meshIn.vertexNormal.data(),
          buffer + view->byteOffset,
          view->byteLength
        );

        accessor = &(*accessors)[primitive.attributes.at(ATT_TEXCOORD.data())];
        view = &(*bufferViews)[accessor->bufferView];
        meshIn.texCoords.resize(accessor->count);
        std::memcpy(
          meshIn.texCoords.data(),
          buffer + view->byteOffset,
          view->byteLength
        );

        accessor = &(*accessors)[primitive.attributes.at(ATT_TANGENT.data())];
        view = &(*bufferViews)[accessor->bufferView];
        meshIn.vertexTangent.resize(accessor->count);
        std::memcpy(
          meshIn.vertexTangent.data(),
          buffer + view->byteOffset,
          view->byteLength
        );
      }
      catch (std::out_of_range e)
      {
	      std::cout << "[Model Compiler] Failed to load critical data from gltf\n";
      }
    }
#endif

  }

  inline void MeshCompiler::BuildHeaders(ModelRef asset) noexcept
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
