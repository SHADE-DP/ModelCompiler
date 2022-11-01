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

namespace SHADE
{

	struct SHVec2
	{
		float x, y;
	};

	struct SHVec3
	{
		float x, y, z;
	};

	struct SHMeshAssetHeader
	{
		uint32_t vertexCount;
		uint32_t indexCount;
		std::string name;
	};

	struct SHMeshAsset
	{
		bool compiled;
		bool changed;

		SHMeshAssetHeader header;

		std::vector<SHVec3> vertexPosition;
		std::vector<SHVec3> vertexTangent;
		std::vector<SHVec3> vertexNormal;
		std::vector<SHVec2> texCoords;
		std::vector<uint32_t> indices;
	};
}