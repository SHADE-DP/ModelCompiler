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

// FMOD Fwd Declare
namespace FMOD
{
	class Sound;
	class System;
	class ChannelGroup;
	class Channel;
}
enum FMOD_RESULT : int;
enum FMOD_SPEAKERMODE : int;

// Typedefs
typedef uint32_t							AssetID;
typedef std::string						AssetName;
typedef std::filesystem::path AssetPath;
typedef unsigned char*				AssetData;
typedef std::string						AssetMetaVersion;
typedef std::string						AssetExtension;
typedef unsigned char					AssetTypeMeta;

typedef FMOD::Sound*					SHSound;

// Asset Meta Version
#define ASSET_META_VER "1.0"

// Asset type enum
enum class AssetType : AssetTypeMeta
{
	INVALID = 0,
	AUDIO = 1,
	SHADER,
	MATERIAL,
	IMAGE,
	TEXTURE,
	MESH,
	SCRIPT,
	SCENE,
	PREFAB,
	AUDIO_WAV,
	DDS
};

//Directory
#ifdef _PUBLISH
#define ASSET_ROOT "Assets"
#else
#define ASSET_ROOT "../../Assets"
#endif


// ASSET EXTENSIONS
#define META_EXTENSION ".shmeta"
#define IMAGE_EXTENSION ".png"
#define AUDIO_EXTENSION ".ogg"
#define AUDIO_WAV_EXTENSION ".wav"
#define SHADER_EXTENSION ".glsl"
#define SCRIPT_EXTENSION ".cs"
#define SCENE_EXTENSION ".SHADE"
#define PREFAB_EXTENSION ".SHPrefab"
#define MATERIAL_EXTENSION ".SHMat"
#define TEXTURE_EXTENSION ".shtex"
#define DDS_EXTENSION ".dds"
#define FBX_EXTENSION ".fbx"
#define GLTF_EXTENSION ".gltf"
#define MESH_EXTENSION ".shmesh"

std::string const EXTENSIONS[] = {
	AUDIO_EXTENSION,
	SHADER_EXTENSION,
	MATERIAL_EXTENSION,
	IMAGE_EXTENSION,
	TEXTURE_EXTENSION,
	DDS_EXTENSION,
	MESH_EXTENSION,
	SCRIPT_EXTENSION,
	SCENE_EXTENSION,
	PREFAB_EXTENSION,
	AUDIO_WAV_EXTENSION,
	FBX_EXTENSION,
	GLTF_EXTENSION
};

// Error flags
#define FILE_NOT_FOUND_ERR "FILE NOT FOUND"
#define META_NOT_FOUND_ERR "META NOT FOUND"
#define ASSET_NOT_FOUND_ERR "ASSET NOT FOUND"
#define EXT_DOES_NOT_EXIST "TYPE DOES NOT HAVE EXTENSION DEFINED"

#endif // !SH_ASSET_MACROS_H
