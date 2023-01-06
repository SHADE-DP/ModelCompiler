/******************************************************************************
 * \file    MeshWriter.cpp
 * \author  Loh Xiao Qi
 * \date    19 November 2022
 * \brief   
 * 
 * \copyright	Copyright (c) 2021 Digipen Institute of Technology. Reproduction
 *		        or disclosure of this file or its contents without the prior
 *			      written consent of Digipen Institute of Technology is prohibited.
 ******************************************************************************/
#include "MeshWriter.h"
#include <fstream>
#include <iostream>
#include <queue>

namespace SH_COMP
{
  void MeshWriter::WriteMeshHeader(std::ofstream& file, MeshDataHeader const& header)
  {
    file.write(
      reinterpret_cast<char const*>(&header),
      sizeof(MeshDataHeader)
    );
  }

  void MeshWriter::WriteMeshData(std::ofstream& file, MeshDataHeader const& header, MeshData const& asset)
  {
    auto const vertexVec3Byte{ sizeof(SHVec3) * header.vertexCount };
    auto const vertexVec2Byte{ sizeof(SHVec2) * header.vertexCount };

    file.write(
      asset.name.c_str(),
      header.charCount
    );

    file.write(
      reinterpret_cast<char const*>(asset.vertexPosition.data()),
      vertexVec3Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.vertexTangent.data()),
      vertexVec3Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.vertexNormal.data()),
      vertexVec3Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.texCoords.data()),
      vertexVec2Byte
    );

    file.write(
      reinterpret_cast<char const*>(asset.indices.data()),
      sizeof(uint32_t) * header.indexCount
    );
  }

  void MeshWriter::WriteAnimHeader(FileReference file, AnimDataHeader const& header)
  {
    auto constexpr intSize = sizeof(uint32_t);

    file.write(
      reinterpret_cast<char const*>(&header.charCount),
      intSize
    );

    file.write(
      reinterpret_cast<char const*>(&header.animNodeCount),
      intSize
    );
    
    file.write(
      reinterpret_cast<char const*>(header.nodeHeaders.data()),
      sizeof(AnimNodeInfo) * header.nodeHeaders.size()
    );
  }

  void MeshWriter::WriteAnimData(FileReference file, AnimDataHeader const& header, AnimData const& data)
  {
    file.write(
      data.name.data(),
      header.charCount
    );

    file.write(
      reinterpret_cast<char const*>(&data.duration),
      sizeof(double)
    );

    file.write(
      reinterpret_cast<char const*>(&data.ticksPerSecond),
      sizeof(double)
    );

		for (auto i{0}; i < header.animNodeCount; ++i)
		{
			WriteAnimNode(file, header.nodeHeaders[i], data.nodeChannels[i]);
		}
  }

  void MeshWriter::WriteAnimNode(FileReference file, AnimNodeInfo const& info, AnimNode const& node)
  {
    file.write(
      node.name.data(),
      info.charCount
    );

    file.write(
      reinterpret_cast<char const*>(&node.pre),
      sizeof(AnimationBehaviour)
    );

    file.write(
      reinterpret_cast<char const*>(&node.post),
      sizeof(AnimationBehaviour)
    );

    file.write(
      reinterpret_cast<char const*>(node.positionKeys.data()),
      sizeof(PositionKey) * node.positionKeys.size()
    );

    file.write(
      reinterpret_cast<char const*>(node.rotationKeys.data()),
      sizeof(RotationKey) * node.rotationKeys.size()
    );

    file.write(
      reinterpret_cast<char const*>(node.scaleKeys.data()),
      sizeof(ScaleKey) * node.scaleKeys.size()
    );
  }

  void MeshWriter::WriteRigHeader(FileReference file, RigDataHeader const& header)
  {
    file.write(
      reinterpret_cast<char const*>(header.nodeCount),
      sizeof(uint32_t)
    );

    file.write(
      reinterpret_cast<char const*>(header.charCounts.data()),
      sizeof(uint32_t) * header.nodeCount
    );
  }

  void MeshWriter::WriteRigData(FileReference file, RigDataHeader const& header,
	  RigData const& data)
  {
    for (auto i {0}; i < header.nodeCount; ++i)
    {
	    file.write(
        data.at(i).name.data(),
        header.charCounts[i]
      );

      file.write(
        reinterpret_cast<char const*>(&data.at(i).transform),
        sizeof(SHMat4)
      );
    }
  }

  void MeshWriter::WriteRigNodes(FileReference file, RigDataHeader const& header, RigNode const* root)
  {
    std::queue<RigNode const*> nodeQueue;
    nodeQueue.push(root);

    while(!nodeQueue.empty())
    {
      auto const node = nodeQueue.front();
      nodeQueue.pop();

	    file.write(
        reinterpret_cast<char const*>(&node->idRef),
        sizeof(uint32_t)
      );

      uint32_t const size { static_cast<uint32_t>(node->children.size()) };
      
	    file.write(
        reinterpret_cast<char const*>(&size),
        sizeof(uint32_t)
      );

      for (auto const& child : node->children)
      {
	      nodeQueue.push(child);
      }
    }
  }

  void MeshWriter::WriteHeaders(FileReference file, ModelConstRef asset)
  {
    for (auto const& header : asset.meshHeaders)
    {
	    WriteMeshHeader(file, header);
    }

    for (auto  const& header : asset.animHeaders)
    {
	    WriteAnimHeader(file, header);
    }
  }

  void MeshWriter::WriteData(FileReference file, ModelConstRef asset)
  {
    for (auto i {0}; i < asset.meshes.size(); ++i)
    {
	    WriteMeshData(file, asset.meshHeaders[i], asset.meshes[i]);
    }
    
    for (auto i {0}; i < asset.anims.size(); ++i)
    {
	    WriteAnimData(file, asset.animHeaders[i], asset.anims[i]);
    }
  }

  void MeshWriter::CompileMeshBinary(AssetPath path, ModelAsset const& asset) noexcept
  {
    std::string newPath{ path.string().substr(0, path.string().find_last_of('.')) };
    newPath += MODEL_EXTENSION;

    std::ofstream file{ newPath, std::ios::out | std::ios::binary | std::ios::trunc };
    if (!file.is_open())
    {
      std::cout << "Unable to open file for write: " << newPath << std::endl;
      return;
    }

    file.write(
      reinterpret_cast<char const*>(&asset.header),
      sizeof(asset.header)
    );

    WriteHeaders(file, asset);
    WriteData(file, asset);

    file.close();
  }
}
