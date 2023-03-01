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
		bool hasWeights;
	};

	struct VertexWeights
	{
		SHVec4 weights;
		SHVec4i joints;
	};

	struct MeshData
	{
		std::string name;

		std::vector<SHVec3> vertexPosition;
		std::vector<SHVec3> vertexTangent;
		std::vector<SHVec3> vertexNormal;
		std::vector<SHVec2> texCoords;
		std::vector<IndexType> indices;

		//Variable data
		std::vector<VertexWeights> weights;
	};
}