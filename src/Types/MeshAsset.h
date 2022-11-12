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
#include <string>

namespace SH_COMP
{

	struct SHVec2
	{
		float x, y;
	};

	struct SHVec3
	{
		float x, y, z;
	};

	struct SHMat4
	{
		float data[16];
	};

	struct MeshDataHeader
	{
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t charCount;
		uint32_t boneCount;
	};

	struct MeshBoneInfo
	{
		uint32_t charCount;
		uint32_t weightCount; // Size should be same as boneCount
	};

	struct BoneWeight
	{
		uint32_t index;
		float weight;
	};

	struct MeshBone
	{
		std::string name;
		SHMat4 offset;
		std::vector<BoneWeight> weights;
	};

	struct MeshData
	{
		std::string name;
		std::vector<SHVec3> vertexPosition;
		std::vector<SHVec3> vertexTangent;
		std::vector<SHVec3> vertexNormal;
		std::vector<SHVec2> texCoords;
		std::vector<uint32_t> indices;
		std::vector<MeshBoneInfo> bonesInfo;
		std::vector<MeshBone> bones;
	};

	struct RigNode
	{
		std::string name;
		uint32_t id;
		SHMat4 transform;
		std::vector<RigNode*> children;
	};

	struct RigData
	{
		RigNode* root;
	};

	struct ModelAssetHeader
	{
		size_t meshCount;
	};

	struct ModelAsset
	{
		ModelAssetHeader header;
		std::vector<MeshDataHeader> headers;
		std::vector<MeshData> meshes;
	};
}