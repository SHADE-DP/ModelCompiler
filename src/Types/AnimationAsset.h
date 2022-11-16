/*************************************************************************//**
 * \file    SHAnimationAsset.h
 * \author  Loh Xiao Qi
 * \date    October 2022
 * \brief    
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/
#pragma once

#include "PseudoMath.h"
#include <vector>
#include <assimp/anim.h>

namespace SH_COMP
{
	enum class AnimationBehaviour : uint8_t
	{
		DEFAULT = 0x0,
		CONSTNAT = 0x1,
		LINEAR = 0x2,
		REPEAT = 0x3
	};

	// Smallest data containers
	struct PositionKey
	{
		float time;
		SHVec3 value;
	};
	
	struct RotationKey
	{
		float time;
		SHVec4 value;
	};
	
	struct ScaleKey
	{
		float time;
		SHVec3 value;
	};

	// Headers for read/write
	struct AnimNodeInfo
	{
		uint32_t charCount;
		uint32_t posKeyCount;
		uint32_t rotKeyCount;
		uint32_t scaKeyCount;
	};

	struct AnimDataHeader
	{
		uint32_t charCount;
		uint32_t animNodeCount;
		std::vector<AnimNodeInfo> nodeHeaders;
	};

	// Main data containers
	struct AnimNode
	{
		std::string name;
		AnimationBehaviour pre;
		AnimationBehaviour post;

		std::vector<PositionKey> positionKeys;
		std::vector<RotationKey> rotationKeys;
		std::vector<ScaleKey> scaleKeys;

	};

	struct AnimData
	{
		std::string name;

		double duration;
		double ticksPerSecond;

		std::vector<AnimNode> nodeChannels;
		//std::vector<aiMeshAnim*> meshChannels;
		//std::vector<aiMeshMorphAnim*> morphMeshChannels;
	};
}