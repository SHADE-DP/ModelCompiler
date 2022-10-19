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

#include <vector>
#include <assimp/anim.h>

namespace SHADE
{
	struct SHAnimationAsset
	{
		std::string name;

		std::vector<aiNodeAnim*> nodeChannels;
		std::vector<aiMeshAnim*> meshChannels;
		std::vector<aiMeshMorphAnim*> morphMeshChannels;

		double duration;
		double ticksPerSecond;
	};
}