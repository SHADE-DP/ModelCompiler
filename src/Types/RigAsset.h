#pragma once

#include <string>
#include <vector>
#include <map>

#include "PseudoMath.h"

namespace SH_COMP
{
	using NodeDataFlag = unsigned char;

	constexpr NodeDataFlag NODE_DATA_ROTATION			= 0b00001;
	constexpr NodeDataFlag NODE_DATA_SCALE				= 0b00010;
	constexpr NodeDataFlag NODE_DATA_TRANSLATION	= 0b00100;
	constexpr NodeDataFlag NODE_DATA_MATRIX				= 0b01000;
	constexpr NodeDataFlag NODE_DATA_WEIGHTS			= 0b10000;

	struct RigDataHeader
	{
		uint32_t nodeCount;
		std::vector<uint32_t> charCounts;
		std::vector<uint32_t> childCount;
		std::vector<NodeDataFlag> dataFlags;
	};

	struct NodeAsset
	{
		std::string name;
		std::vector<IndexType> children;
		std::vector<double>
			rotation,
			scale,
			translation,
			matrix,
			weights;
		SHMat4 inverseBindMatrix;
	};

	struct RigData
	{
		RigDataHeader header;
		std::vector<NodeAsset> nodes;
	};
}
