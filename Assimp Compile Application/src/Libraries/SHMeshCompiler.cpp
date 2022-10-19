/*************************************************************************//**
 * \file    SHMeshCompiler.cpp
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
#include "SHMeshCompiler.h"

#include <fstream>
#include <iostream>

std::string SHADE::SHMeshCompiler::CompileMeshBinary(SHMeshAsset const& asset) noexcept
{
	std::string newPath{ asset.parentPath };
	newPath += asset.header.meshName + MESH_EXTENSION;

	std::ofstream file{ newPath, std::ios::out | std::ios::binary | std::ios::trunc };
	if (!file.is_open())
	{
		std::cout << "Unable to open file for writing mesh file: " << asset.parentPath << std::endl;
	}

	file.write(
		reinterpret_cast<char const*>(&(asset.header.vertexCount)),
		sizeof(uint32_t)
	);

	file.write(
		reinterpret_cast<const char*>(&(asset.header.indexCount)),
		sizeof(uint32_t)
	);

	auto const vertexVec3Byte {sizeof(SHVec3) * asset.header.vertexCount};
	auto const vertexVec2Byte {sizeof(SHVec2) * asset.header.vertexCount};

	file.write(
		reinterpret_cast<char const*>(asset.vertexPosition.data()),
		vertexVec3Byte
	);
	
	file.write(
		reinterpret_cast<char const*>(asset.vertexTangent.data()),
		vertexVec3Byte
	);
	
	file.write(
		reinterpret_cast<char const*>(asset.vertexNormal.data()),
		vertexVec3Byte
	);
	
	file.write(
		reinterpret_cast<char const*>(asset.texCoords.data()),
		vertexVec2Byte
	);

	file.write(
		reinterpret_cast<char const*>(asset.indices.data()),
		sizeof(uint32_t) * asset.header.indexCount
	);

	file.close();

	return newPath;
}
