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

enum class BUFFER_TARGET : int
{
	ARRAY_BUFFER = 34962,
	ELEMENT_ARRAY_BUFFER = 34963
};

enum class ACCESSOR_COMPONENT_TYPE : int
{
	BYTE = 5120,
	U_BYTE = 5121,
	SHORT = 5122,
	U_SHORT = 5123,
	U_INT = 5125,
	FLOAT = 5126
};

constexpr size_t SizeOfType(ACCESSOR_COMPONENT_TYPE type)
{
	switch(type)
	{
	case ACCESSOR_COMPONENT_TYPE::BYTE:
	case ACCESSOR_COMPONENT_TYPE::U_BYTE:
		return sizeof(char);
	case ACCESSOR_COMPONENT_TYPE::SHORT:
	case ACCESSOR_COMPONENT_TYPE::U_SHORT:
		return sizeof(short);
	case ACCESSOR_COMPONENT_TYPE::U_INT:
	case ACCESSOR_COMPONENT_TYPE::FLOAT:
		return sizeof(float);
	default:
		return 0;
	}
}

enum class ACCESSOR_DATA_TYPE : int
{
	SCALAR = 64 + 1,
	VEC2 = 2,
	VEC3 = 3,
	VEC4 = 4,
	MAT2 = 32 + 2,
	MAT3 = 32 + 3,
	MAT4 = 32 + 4,
	VECTOR = 64 + 4,
	MATRIX = 64 + 16
};

constexpr size_t CountOfType(ACCESSOR_DATA_TYPE type)
{
	switch (type)
	{
	case ACCESSOR_DATA_TYPE::SCALAR:
		return 1;
	case ACCESSOR_DATA_TYPE::VEC2:
		return 2;
	case ACCESSOR_DATA_TYPE::VEC3:
		return 3;
	case ACCESSOR_DATA_TYPE::VEC4:
		return 4;
	case ACCESSOR_DATA_TYPE::MAT2:
		return 2 * 2;
	case ACCESSOR_DATA_TYPE::MAT3:
		return 3 * 3;
	case ACCESSOR_DATA_TYPE::MAT4:
		return 4 * 4;
	default:
		return 0;
	}
}

enum class PRIMITIVE_MODE : int
{
	POINT = 0,
	LINE = 1,
	LINE_LOOP = 2,
	LINE_STRIP = 3,
	TRIANGLE = 4,
	TRIANGLE_STRIP = 5,
	TRIANLE_FAN = 6
};

constexpr std::string_view TRANSLATION_PATH{ "translation" };
constexpr std::string_view SCALE_PATH{ "scale" };
constexpr std::string_view ROTATION_PATH{ "rotation" };
constexpr std::string_view WEIGHTS_PATH{ "weights" };

constexpr std::string_view LINEAR_INTERPOLATION{ "LINEAR" };
constexpr std::string_view STEP_INTERPOLATION{ "STEP" };
constexpr std::string_view CUBICSPLINE_INTERPOLATION{ "CUBICSPLINE" };

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

// ATTRIBUTE NAMES
// BASIC NEEDED
constexpr std::string_view ATT_POSITION {"POSITION"};
constexpr std::string_view ATT_NORMAL { "NORMAL" };
constexpr std::string_view ATT_TANGENT { "TANGENT" };
constexpr std::string_view ATT_TEXCOORD { "TEXCOORD_0" };

// VARIABLE ATTRIBUTES
constexpr std::string_view ATT_WEIGHTS { "WEIGHTS_0" };
constexpr std::string_view ATT_JOINT{ "JOINTS_0" };
constexpr std::string_view ATT_COLOUR{ "COLOR_0" };

constexpr std::string_view EXTERNALS[] = {
	FBX_EXTENSION,
	GLTF_EXTENSION
};
