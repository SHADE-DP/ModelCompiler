/*************************************************************************//**
 * \file    SHAssimpLibrary.h
 * \author  Loh Xiao Qi
 * \date    October 2022
 * \brief    
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/

#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include "../SHAssetMacros.h"
#include "../Types/SHMeshAsset.h"
#include "../Types/SHAnimationAsset.h"

namespace SHADE
{
	using MeshVector = std::vector<SHMeshAsset>;
	using AnimVector = std::vector<SHAnimationAsset>;

	class SHAssimpLibrary
	{
	private:
		using MeshVectorRef = std::vector<SHMeshAsset>&;
		using AnimVectorRef = std::vector<SHAnimationAsset>&;

		static Assimp::Importer aiImporter;
		static void ProcessNode(aiNode const& node, aiScene const& scene,MeshVectorRef meshes) noexcept;
		static void ExtractAnimations(aiScene const& scene, AnimVectorRef anims) noexcept;
		static void ProcessMesh(aiMesh const& mesh, SHMeshAsset& meshDest) noexcept;

	public:
		static void LoadFromFile(AssetPath path, MeshVectorRef meshes, AnimVectorRef anims) noexcept;
	};
}