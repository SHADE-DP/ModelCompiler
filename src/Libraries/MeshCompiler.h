/*************************************************************************//**
 * \file    SHMeshCompiler.h
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
#include "Types/MeshAsset.h"
#include "AssetMacros.h"

namespace SH_COMP
{
  class MeshCompiler
  {

    using MeshVectorRef = std::vector<MeshData>&;
    using AnimVectorRef = std::vector<AnimationAsset>&;

    static Assimp::Importer aiImporter;

    static void ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes) noexcept;
    static void ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept;
    static void GetMesh(aiMesh const& mesh, MeshData& meshData) noexcept;
    static void BuildHeaders(MeshAsset& asset) noexcept;

    static void WriteMeshHeader(std::ofstream& file, MeshDataHeader const& header);
    static void WriteMeshData(std::ofstream& file, MeshDataHeader const& header, MeshData const& data);

    static void LoadFromFile(AssetPath path, MeshAsset& asset) noexcept;
    static void CompileMeshBinary(AssetPath path, MeshAsset const& asset) noexcept;

  public:
    static void LoadAndCompile(AssetPath path) noexcept;
	};
}