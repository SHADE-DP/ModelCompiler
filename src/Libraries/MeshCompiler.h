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

#include <vector>

#include "Types/AnimationAsset.h"
#include "Types/ModelAsset.h"
#include "AssetMacros.h"

//Forward Declare
namespace tinygltf
{
	struct Accessor;
  struct BufferView;
	class Model;
}

namespace SH_COMP
{
  using MeshVectorRef = std::vector<MeshData>&;
  using AnimVectorRef = std::vector<AnimData>&;

  using ModelRef = ModelAsset&;
  using ModelData = tinygltf::Model;
  using AccessorReference = std::vector<tinygltf::Accessor> const*;
  using BufferViewReference = std::vector<tinygltf::BufferView> const*;
  using BufferData = unsigned char const*;

  class MeshCompiler
  {

    static AccessorReference accessors;
    static BufferViewReference bufferViews;
    static BufferData buffer;

  	static inline void LoadFromFile(AssetPath path, ModelRef asset) noexcept;

    static inline void ProcessModel(ModelData const& model, ModelRef asset) noexcept;
    static inline void ProcessAnimations(ModelData const& model, ModelRef asset) noexcept;

    static inline void BuildHeaders(ModelRef asset) noexcept;

    template<typename T>
    static void FetchData(int accessorID, std::vector<T>& dst);

    template<typename T>
    static void FetchChannelKeyFrame(int targetNode, int inputAcc, int outputAcc, int nodeTarget, std::vector<T>& dst);
  public:
    static void LoadAndCompile(AssetPath path) noexcept;
	};
}

#include "MeshCompiler.hpp"
