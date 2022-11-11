/******************************************************************************
 * \file    AssetMacros.h
 * \author  Loh Xiao Qi
 * \brief   Macros and typedefs for assets
 * 
 * \copyright	Copyright (c) 2022 Digipen Institute of Technology. Reproduction
 *						or disclosure of this file or its contents without the prior
 *						written consent of Digipen Institute of Technology is prohibited
 ******************************************************************************/
#pragma once

#include <filesystem>

// Typedefs
typedef std::filesystem::path AssetPath;

//Directory
#ifdef _PUBLISH
constexpr std::string_view ASSET_ROOT{ "Assets" };
constexpr std::string_view BUILT_IN_ASSET_ROOT{ "Built_In" };
#else
constexpr std::string_view ASSET_ROOT{ "../../Assets" };
constexpr std::string_view BUILT_IN_ASSET_ROOT{ "../../Built_In" };
#endif

// ASSET EXTENSIONS
constexpr std::string_view MODEL_EXTENSION {".shmodel"};

// EXTERNAL EXTENSIONS
constexpr std::string_view FBX_EXTENSION{ ".fbx" };
constexpr std::string_view GLTF_EXTENSION{ ".gltf" };

constexpr std::string_view EXTERNALS[] = {
	FBX_EXTENSION,
	GLTF_EXTENSION
};
