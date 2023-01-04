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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

#include "Types/AnimationAsset.h"
#include "Types/ModelAsset.h"
#include "AssetMacros.h"

namespace SH_COMP
{
  class MeshCompiler
  {

    using MeshVectorRef = std::vector<MeshData>&;
    using AnimVectorRef = std::vector<AnimData>&;

    using ModelRef = ModelAsset&;

    static Assimp::Importer aiImporter;
    static uint32_t rigNodeIDCounter;

    static void ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes, RigData& rig) noexcept;
    static void GetMesh(aiMesh const& mesh, MeshData& meshData) noexcept;
    static void BuildHeaders(ModelRef asset) noexcept;

  	static void BuildArmature(aiNode const& node, RigData& rig) noexcept;
    static void ParseAnimations(aiScene const& scene, std::vector<AnimData>& anims) noexcept;

  	static void LoadFromFile(AssetPath path, ModelRef asset) noexcept;

  public:
    static void LoadAndCompile(AssetPath path) noexcept;
	};
}
