/*************************************************************************//**
 * \file    SHMeshCompiler.h
 * \author  Loh Xiao Qi
 * \date    30 September 2022
 * \brief   Library to write data in SHMeshAsset into binary file for faster
 *					loading in the future
 * 
 *
 * Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 * disclosure of this file or its contents without the prior written consent
 * of DigiPen Institute of Technology is prohibited.
 *****************************************************************************/
#pragma once

#include "../Types/SHMeshAsset.h"
#include "../SHAssetMacros.h"

namespace SHADE
{
	class SHMeshCompiler
	{
	private:
	public:
		static std::string CompileMeshBinary(SHMeshAsset const& asset) noexcept;
	};
}