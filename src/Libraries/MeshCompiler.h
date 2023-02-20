/*************************************************************************//**
 * \file    MeshCompiler.h
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
#pragma once

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_USE_CPP14

#include <vector>

#include "Types/AnimationAsset.h"
#include "Types/ModelAsset.h"
#include "AssetMacros.h"

//Forward Declare
namespace tinygltf
{
	class Model;
}

namespace SH_COMP
{
  class tinygltf::Model;
  class MeshCompiler
  {

    using MeshVectorRef = std::vector<MeshData>&;
    using AnimVectorRef = std::vector<AnimData>&;

    using ModelRef = ModelAsset&;
    using ModelData = tinygltf::Model;
    
    static uint32_t rigNodeIDCounter;

  	static void LoadFromFile(AssetPath path, ModelRef asset) noexcept;


    static void ProcessModel(ModelData const&, ModelRef asset) noexcept;

  public:
    static void LoadAndCompile(AssetPath path) noexcept;
	};
}
