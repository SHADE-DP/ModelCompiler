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
		std::map<uint32_t, RigNodeData> nodeDataCollection;
		RigNode* root;
	};
}
