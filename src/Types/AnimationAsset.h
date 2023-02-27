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

	struct AnimDataHeader
	{
		uint32_t charCount;
		uint32_t animNodeCount;
		uint32_t frameCount;
	};

	// Main data containers
	struct AnimNode
	{
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

		//One node represents the animation transforms for one bone in the rig
		std::vector<AnimNode> nodes;
	};
}