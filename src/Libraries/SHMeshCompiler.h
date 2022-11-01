/*************************************************************************//**
 * \file    SHMeshCompiler.h
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
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

#include "Types/SHAnimationAsset.h"
#include "Types/SHMeshAsset.h"
#include "SHAssetMacros.h"

namespace SHADE
{
	class SHMeshCompiler
	{

    using MeshVectorRef = std::vector<SHMeshAsset*>&;
    using AnimVectorRef = std::vector<SHAnimationAsset*>&;

    static Assimp::Importer aiImporter;
    static void ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes) noexcept;
    static void ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept;
    static SHMeshAsset* ProcessMesh(aiMesh const& mesh) noexcept;
    static void LoadFromFile(AssetPath path, MeshVectorRef meshes, AnimVectorRef anims) noexcept;
    static std::optional<AssetPath> CompileMeshBinary(SHMeshAsset const& asset, AssetPath path) noexcept;

  public:
    static void LoadAndCompile(AssetPath path) noexcept;
	};
}