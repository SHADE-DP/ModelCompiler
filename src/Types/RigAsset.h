#pragma once

#include <string>
#include <vector>
#include <map>

#include "PseudoMath.h"

namespace SH_COMP
{
	struct RigDataHeader
	{
		uint32_t nodeCount;
		std::vector<uint32_t> charCounts;
	};

	struct RigNodeData
	{
		RigNodeData(const char* cstr, SHMat4 mat)
			:name {cstr}, transform{mat} {}

		std::string name;
		SHMat4 transform;
		std::vector<RigNodeData*> children;
	};

	struct RigNodeDataWrite
	{
		std::string name;
		SHMat4 transform;
	};

	struct RigWriteNode
	{
		uint32_t id;
		std::vector<RigWriteNode*> children;
	};

	struct RigData
	{
		RigDataHeader header;
		// std::map<uint32_t, RigNodeData> nodeDataCollection;
		RigNodeData* root;
	};
}
