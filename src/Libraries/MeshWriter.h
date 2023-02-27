/******************************************************************************
 * \file    MeshWriter.h
 * \author  Loh Xiao Qi
 * \date    19 November 2022
 * \brief   
 * 
 * \copyright	Copyright (c) 2021 Digipen Institute of Technology. Reproduction
 *		        or disclosure of this file or its contents without the prior
 *			      written consent of Digipen Institute of Technology is prohibited.
 ******************************************************************************/
#pragma once

#include "AssetMacros.h"
#include "Types/ModelAsset.h"

namespace SH_COMP
{
	struct MeshWriter
	{
    using FileReference = std::ofstream&;
    using ModelConstRef = ModelAsset const&;

    static void WriteMeshData(FileReference file, std::vector<MeshDataHeader> const& headers, std::vector<MeshData> const& meshes);
    static void WriteAnimData(FileReference file, std::vector<AnimDataHeader> const& headers, std::vector<AnimData> const& anims);
    static void WriteAnimNode(FileReference file, AnimNode const& node);

    static void WriteRig(FileReference file, RigData const& data);
    static void WriteRigHeader(FileReference file, RigDataHeader const& header);
    static void WriteRigNodeData(FileReference file, RigData const& rig);

    static void WriteHeaders(FileReference file, ModelConstRef asset);
    static void WriteData(FileReference file, ModelConstRef asset);

		static void CompileMeshBinary(AssetPath path, ModelConstRef asset) noexcept;
	};
}
