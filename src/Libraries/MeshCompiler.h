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
#include "Types/ModelAsset.h"
#include "AssetMacros.h"

namespace SH_COMP
{
  class MeshCompiler
  {

    using MeshVectorRef = std::vector<MeshData>&;
    using AnimVectorRef = std::vector<AnimationAsset>&;

    static Assimp::Importer aiImporter;

    static void ProcessNode(aiNode const& node, aiScene const& scene, MeshVectorRef meshes, RigNode*& root) noexcept;
    //static void ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept;
    static void GetMesh(aiMesh const& mesh, MeshData& meshData) noexcept;
    static void BuildHeaders(ModelAsset& asset) noexcept;

    static void WriteMeshHeader(std::ofstream& file, MeshDataHeader const& header);
    static void WriteMeshData(std::ofstream& file, MeshDataHeader const& header, MeshData const& asset);

    static void LoadFromFile(AssetPath path, ModelAsset& asset) noexcept;
    static void CompileMeshBinary(AssetPath path, ModelAsset const& asset) noexcept;

    static void BuildArmature(aiNode const& node, RigNode*& root) noexcept;
    static void CopyNode(aiNode const& source, RigNode* parent) noexcept;

    static void ParseAnimations(aiScene const& scene, std::vector<AnimationAsset>& anims) noexcept;
  public:
    static void LoadAndCompile(AssetPath path) noexcept;
	};
}