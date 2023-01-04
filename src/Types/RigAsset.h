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
	};

	struct RigNode
	{
		uint32_t idRef;
		std::vector<RigNode*> children;
	};

	struct RigData
	{
		RigDataHeader header;
		std::map<uint32_t, RigNodeData> nodeDataCollection;
		RigNode* root;
	};
}
