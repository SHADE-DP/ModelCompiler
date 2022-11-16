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
	struct AnimationKey
	{
		float time;
		SHVec4 value;
	};
	struct AnimationNode
	{
		std::string name;
		std::vector<AnimationKey> positionKeys;
		std::vector<AnimationKey> rotationKeys;
		std::vector<AnimationKey> scaleKeys;
	};

	struct AnimationAsset
	{
		std::string name;

		std::vector<AnimationNode> nodeChannels;
		//std::vector<aiMeshAnim*> meshChannels;
		//std::vector<aiMeshMorphAnim*> morphMeshChannels;

		double duration;
		double ticksPerSecond;
	};
}