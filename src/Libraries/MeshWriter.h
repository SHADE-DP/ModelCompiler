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

		static void WriteMeshHeader(FileReference file, MeshDataHeader const& header);
    static void WriteMeshData(FileReference file, MeshDataHeader const& header, MeshData const& asset);

		static void WriteAnimHeader(FileReference file, AnimDataHeader const& header);
    static void WriteAnimData(FileReference file, AnimDataHeader const& header, AnimData const& data);
    static void WriteAnimNode(FileReference file, AnimNodeInfo const& info, AnimNode const& node);

    static void WriteRigHeader(FileReference file, RigDataHeader const& header);
    static void WriteRigData(FileReference file,  RigDataHeader const& header, std::map<uint32_t, RigNodeData> const& data);
    static void WriteRigNodes(FileReference file,  RigDataHeader const& header, RigNode const* root);

    static void WriteHeaders(FileReference file, ModelConstRef asset);
    static void WriteData(FileReference file, ModelConstRef asset);

		static void CompileMeshBinary(AssetPath path, ModelConstRef asset) noexcept;
	};
}
