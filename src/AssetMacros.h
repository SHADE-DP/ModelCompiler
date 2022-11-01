/******************************************************************************
 * \file    SHAssetMacros.h
 * \author  Loh Xiao Qi
 * \brief   Macros and typedefs for assets
 * 
 * \copyright	Copyright (c) 2022 Digipen Institute of Technology. Reproduction
 *						or disclosure of this file or its contents without the prior
 *						written consent of Digipen Institute of Technology is prohibited
 ******************************************************************************/
#ifndef SH_ASSET_MACROS_H
#define SH_ASSET_MACROS_H

#include <cstdint>
#include <string>
#include <filesystem>

// Typedefs
typedef uint32_t							AssetID;
typedef std::string						AssetName;
typedef std::filesystem::path AssetPath;
typedef unsigned char*				AssetData;
typedef std::string						AssetMetaVersion;
typedef std::string						AssetExtension;
typedef size_t					AssetTypeMeta;

//Directory
#ifdef _PUBLISH
constexpr std::string_view ASSET_ROOT{ "Assets" };
constexpr std::string_view BUILT_IN_ASSET_ROOT{ "Built_In" };
#else
constexpr std::string_view ASSET_ROOT{ "../../Assets" };
constexpr std::string_view BUILT_IN_ASSET_ROOT{ "../../Built_In" };
#endif

// ASSET EXTENSIONS
constexpr std::string_view TEXTURE_EXTENSION {".shtex"};
constexpr std::string_view MESH_EXTENSION {".shmesh"};

// EXTERNAL EXTENSIONS
constexpr std::string_view FBX_EXTENSION{ ".fbx" };
constexpr std::string_view GLTF_EXTENSION{ ".gltf" };

constexpr std::string_view EXTERNALS[] = {
	FBX_EXTENSION,
	GLTF_EXTENSION
};

// Error flags
constexpr std::string_view FILE_NOT_FOUND_ERR {"FILE NOT FOUND"};
constexpr std::string_view META_NOT_FOUND_ERR {"META NOT FOUND"};
constexpr std::string_view ASSET_NOT_FOUND_ERR {"ASSET NOT FOUND"};
constexpr std::string_view EXT_DOES_NOT_EXIST {"TYPE DOES NOT HAVE EXTENSION DEFINED"};

#endif // !SH_ASSET_MACROS_H
