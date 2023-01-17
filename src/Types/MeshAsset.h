#pragma once

#include <vector>
#include <string>

#include "PseudoMath.h"

namespace SH_COMP
{
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
		uint32_t weightCount;
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
}