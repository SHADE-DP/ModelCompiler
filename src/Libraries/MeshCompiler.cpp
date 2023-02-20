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

#include <fstream>
#include <iostream>

#include "tiny_gltf.h"
#include <map>
#include <stack>

namespace SH_COMP
{

  uint32_t MeshCompiler::rigNodeIDCounter { 0 };

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
    auto const& accessors { data.accessors };
    auto const& bufferViews { data.bufferViews };
    auto const& bufferData { data.buffers[0].data.data() };

    for (auto const& mesh : data.meshes)
    {
      auto const& primitive { mesh.primitives[0] };
      auto& meshIn {asset.meshes.emplace_back()};
      meshIn.name = mesh.name;

      try
      {
				// Get Accessors
	      auto const& positionAccessor { accessors[primitive.attributes.at(ATT_POSITION.data())]};
	      auto const& normalAccessor { accessors[primitive.attributes.at(ATT_NORMAL.data())]};
	      auto const& tangentAccessor { accessors[primitive.attributes.at(ATT_TANGENT.data())]};

        meshIn.vertexPosition.resize(positionAccessor.count);
        auto const& positionView { bufferViews[positionAccessor.bufferView] };
        std::memcpy(
          meshIn.vertexPosition.data(), 
          bufferData + positionView.byteOffset,
          positionView.byteLength
        );

        meshIn.vertexNormal.resize(normalAccessor.count);
      }
      catch (std::out_of_range e)
      {
	      std::cout << "[Model Compiler] Failed to load gltf\n";
      }
    }
  }

  void MeshCompiler::LoadAndCompile(AssetPath path) noexcept
  {
    auto const asset = new ModelAsset();

    LoadFromFile(path, *asset);
    MeshWriter::CompileMeshBinary(path, *asset);

    delete asset;
  }
}
