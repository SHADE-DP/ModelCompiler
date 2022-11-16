/*************************************************************************//**
 * \file    SHMeshAsset.h
 * \author  Loh Xiao Qi
 * \date    30 September 2022
 * \brief		Struct to contain ready data for loading into GPU. Also used for
 *					compilation into binary files
 * 
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/
#pragma once

#include <vector>

#include "MeshAsset.h"
#include "AnimationAsset.h"
#include "RigAsset.h"

namespace SH_COMP
{

	struct ModelAssetHeader
	{
		size_t meshCount;
		size_t animCount;
	};

	struct ModelAsset
	{
		ModelAssetHeader header;
		RigData rig;

		std::vector<MeshDataHeader> meshHeaders;
		std::vector<AnimDataHeader> animHeaders;
		
		std::vector<MeshData> meshes;
		std::vector<AnimData> anims;
	};
}