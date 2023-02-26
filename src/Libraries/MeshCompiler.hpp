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
#include <algorithm>

#include "tiny_gltf.h"
#include <map>
#include <stack>

namespace SH_COMP
{
  AccessorReference MeshCompiler::accessors{ nullptr };
  BufferViewReference MeshCompiler::bufferViews{ nullptr };
  BufferData MeshCompiler::buffer{ nullptr };

  inline void MeshCompiler::LoadFromFile(AssetPath path, ModelRef asset) noexcept
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
    ProcessAnimationChannels(model, asset);
  }

  inline void MeshCompiler::ProcessModel(ModelData const& data, ModelRef asset) noexcept
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
        FetchData(primitive.attributes.at(ATT_POSITION.data()), meshIn.vertexPosition);
        FetchData(primitive.attributes.at(ATT_NORMAL.data()), meshIn.vertexNormal);
        FetchData(primitive.attributes.at(ATT_TEXCOORD.data()), meshIn.texCoords);
        FetchData(primitive.indices, meshIn.indices);

        std::vector<SHVec4> intermediate;
        FetchData(primitive.attributes.at(ATT_TANGENT.data()), intermediate);
        meshIn.vertexTangent.resize(intermediate.size());
        std::ranges::transform(
          intermediate,
					meshIn.vertexTangent.begin(),
          [](auto const& inTan)
		       {
		         return SHVec3{ inTan.x, inTan.y, inTan.z };
		       }
        );
      }
      catch (std::out_of_range e)
      {
	      std::cout << "[Model Compiler] Failed to load critical data from gltf\n";
      }
    }
#endif

  }

  template <typename T>
  void MeshCompiler::FetchData(int accessorID, std::vector<T>& dst)
  {
    auto const& accessor = (*accessors)[accessorID];
    auto const& view = (*bufferViews)[accessor.bufferView];
    auto const typeIdentifier{ static_cast<ACCESSOR_COMPONENT_TYPE>(accessor.componentType) };
    auto const sizeIdentifier{ SizeOfType(typeIdentifier) };
    auto const componentCount{ CountOfType(static_cast<ACCESSOR_DATA_TYPE>(accessor.type))};
    auto const totalStrideBytes{ sizeIdentifier * componentCount };
    dst.resize(accessor.count);
    if (sizeof(T) == totalStrideBytes)
    {
      std::memcpy(
        dst.data(),
        buffer + view.byteOffset,
        view.byteLength
      );
      return;
    }

    std::vector<std::byte> tempData(view.byteLength);
    std::memcpy(
      tempData.data(),
      buffer + view.byteOffset,
      view.byteLength
    );

    auto srcPtr{ tempData.data() };
    T* dstPtr{ dst.data() };
    size_t index{ 0 };
    for (auto i{0}; i < accessor.count; ++i, ++index)
    {
      auto srcCompPtr{ srcPtr };
      auto dstCompPtr{ reinterpret_cast<IndexType*>(dstPtr)};
      for (auto j{0}; j < componentCount; ++j)
      {
        std::memcpy(
          dstPtr,
          srcPtr,
          sizeIdentifier
        );

        srcCompPtr += sizeIdentifier;
        ++dstCompPtr;
      }

      srcPtr += totalStrideBytes;

      ++dstPtr;
    }
  }

  template <typename T>
  void MeshCompiler::FetchChannelKeyFrame(int targetNode, int inputAcc, int outputAcc, int nodeTarget, std::vector<T>& dst)
  {
    // ONLY ALLOW THIS FUNCTION TO BE USED ON KEY DATA STRUCT
    static_assert(std::derived_from<T, KeyBase> == true);

    std::vector<float> inputVec;
    std::vector<SHVec3> outputVec;
    FetchData(inputAcc, inputVec);
    FetchData(outputAcc, outputVec);

    dst.resize(inputVec.size());

    std::ranges::transform(
      inputVec,
      outputVec,
      dst.begin(),
      [](float const& time, SHVec3 const& value)->T
      {
        return { time, value };
      }
    );
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

  inline void MeshCompiler::ProcessAnimationChannels(ModelData const& model, ModelRef asset) noexcept
  {
    asset.anims.resize(model.animations.size());
    for (auto i {0}; i < model.animations.size(); ++i)
    {
      auto const& animData{ model.animations[i] };
      auto& anim{ asset.anims[i] };

      anim.name = animData.name;

      for (auto const& channel : animData.channels)
      {
        auto const& sampler{ animData.samplers[channel.sampler] };

        // Resize nodes vector to latest largest index called
        if (anim.nodes.size() <= channel.target_node)
          anim.nodes.resize(channel.target_node + 1);
        
        if (channel.target_path == TRANSLATION_PATH.data())
          FetchChannelKeyFrame(channel.target_node, sampler.input, sampler.output, channel.target_node, anim.nodes[channel.target_node].positionKeys);
        else if (channel.target_path == SCALE_PATH.data())
          FetchChannelKeyFrame(channel.target_node, sampler.input, sampler.output, channel.target_node, anim.nodes[channel.target_node].scaleKeys);
        else if (channel.target_path == ROTATION_PATH.data())
          FetchChannelKeyFrame(channel.target_node, sampler.input, sampler.output, channel.target_node, anim.nodes[channel.target_node].rotationKeys);

        anim.nodes[channel.target_node].interpolation =
	        sampler.interpolation == LINEAR_INTERPOLATION.data() ? AnimationInterpolation::LINEAR :
	        sampler.interpolation == STEP_INTERPOLATION.data() ? AnimationInterpolation::STEP :
	        sampler.interpolation == CUBICSPLINE_INTERPOLATION.data() ? AnimationInterpolation::CUBICSPLINE :
	        AnimationInterpolation::DEFAULT;
      }

      std::cout << "all anim channels copied\n";
    }
  }

  inline void MeshCompiler::ProcessNodes(ModelData const& model, ModelRef asset) noexcept
  {

  }
}
