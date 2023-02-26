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
#include <string>

namespace SH_COMP
{
	enum class AnimationInterpolation : uint8_t
	{
		DEFAULT = 0x1,
		LINEAR = 0x1,
		STEP = 0x2,
		CUBICSPLINE = 0x3
	};

	// Base 
	struct KeyBase
	{
		float time;
		SHVec3 value;
	};

	// Smallest data containers
	struct PositionKey :KeyBase {};
	
	struct RotationKey : KeyBase {};
	
	struct ScaleKey :KeyBase {};

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
		AnimationInterpolation interpolation;

		std::vector<PositionKey> positionKeys;
		std::vector<RotationKey> rotationKeys;
		std::vector<ScaleKey> scaleKeys;
	};

	struct AnimData
	{
		std::string name;

		double duration;
		double ticksPerSecond;

		std::vector<AnimNode> nodes;
		//std::vector<aiMeshAnim*> meshChannels;
		//std::vector<aiMeshMorphAnim*> morphMeshChannels;
	};
}