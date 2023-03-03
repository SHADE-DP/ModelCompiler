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
#include <stack>
#include <queue>

namespace SH_COMP
{
  void MeshWriter::WriteMeshData(FileReference file, std::vector<MeshDataHeader> const& headers,
	  std::vector<MeshData> const& meshes)
  {
    for (auto i {0}; i < headers.size(); ++i)
    {
      auto const& header = headers[i];
      auto const& asset = meshes[i];

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

      if (header.hasWeights)
      {
        file.write(
          reinterpret_cast<char const*>(asset.weights.data()),
          sizeof(SHVec4) * header.vertexCount
        );
        file.write(
          reinterpret_cast<char const*>(asset.joints.data()),
          sizeof(SHVec4i) * header.vertexCount
        );
      }
    }
  }

  void MeshWriter::WriteAnimData(
    FileReference file, 
    std::vector<AnimDataHeader> const& headers,
	  std::vector<AnimData> const& anims
  )
  {
    
    for (auto i {0}; i < headers.size(); ++i)
    {
      auto const& header = headers[i];
      auto const& data = anims[i];
    
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

      for (auto const& node : data.nodes)
      {
        WriteAnimNode(file, node);
      }
    }
  }

  void MeshWriter::WriteAnimNode(FileReference file, AnimNode const& node)
  {
    file.write(
      reinterpret_cast<char const*>(&node.interpolation),
      sizeof(AnimationInterpolation)
    );

    uint32_t const keySize = node.positionKeys.size();

    file.write(
      reinterpret_cast<char const*>(node.positionKeys.data()),
      sizeof(PositionKey) * keySize
    );

    file.write(
      reinterpret_cast<char const*>(node.rotationKeys.data()),
      sizeof(RotationKey) * keySize
    );

    file.write(
      reinterpret_cast<char const*>(node.scaleKeys.data()),
      sizeof(ScaleKey) * keySize
    );
  }

  void MeshWriter::WriteRig(FileReference file, RigData const& data)
  {
    WriteRigHeader(file, data.header);
    WriteRigNodeData(file, data);
    WriteRigStructure(file, data);
  }

  void MeshWriter::WriteRigHeader(FileReference file, RigDataHeader const& header)
  {
    file.write(
      reinterpret_cast<char const*>(&header.nodeCount),
      sizeof(uint32_t)
    );

    file.write(
      reinterpret_cast<char const*>(&header.startNode),
      sizeof(uint32_t)
    );

    file.write(
      reinterpret_cast<char const*>(header.charCounts.data()),
      sizeof(uint32_t) * header.nodeCount
    );
  }

  void MeshWriter::WriteRigNodeData(FileReference file, RigData const& rig)
  {
    std::vector<IndexType> treeNodeWrite;
    for (auto const& node : rig.nodes)
    {
      file.write(
        node.name.data(),
        node.name.size()
      );

      file.write(
        reinterpret_cast<char const*>(&node.inverseBindMatrix),
        sizeof(SHMat4)
      );

      //Build data flags
      NodeDataFlag flag{ 0 };

      if (!node.rotation.empty())
        flag |= NODE_DATA_ROTATION;
      if (!node.scale.empty())
        flag |= NODE_DATA_SCALE;
      if (!node.translation.empty())
        flag |= NODE_DATA_TRANSLATION;
      if (!node.matrix.empty())
        flag |= NODE_DATA_MATRIX;

      file.put(flag);

      file.write(
        reinterpret_cast<char const*>(node.rotation.data()),
        sizeof(double) * node.rotation.size()
      );

      file.write(
        reinterpret_cast<char const*>(node.scale.data()),
        sizeof(double) * node.scale.size()
      );

      file.write(
        reinterpret_cast<char const*>(node.translation.data()),
        sizeof(double) * node.translation.size()
      );

      file.write(
        reinterpret_cast<char const*>(node.matrix.data()),
        sizeof(double) * node.matrix.size()
      );
    }
  }

  void MeshWriter::WriteRigStructure(FileReference file, RigData const& rig)
  {
    std::queue<std::pair<IndexType, NodeAsset const*>> nodeQueue;
    nodeQueue.push(
      std::make_pair(
        rig.header.startNode,
        rig.nodes.data() + rig.header.startNode
      )
    );

    while (!nodeQueue.empty())
    {
      auto const currentPair = nodeQueue.front();
      auto const& node = *currentPair.second;
      nodeQueue.pop();

      file.write(
        reinterpret_cast<char const*>(&currentPair.first),
        sizeof(IndexType)
      );

      auto const childCount{ static_cast<uint32_t>(node.children.size()) };
      file.write(
        reinterpret_cast<char const*>(&childCount),
        sizeof(uint32_t)
      );

      for (auto const& child : node.children)
      {
        nodeQueue.push(std::make_pair(
          child,
          rig.nodes.data() + child
        ));
      }
    }
  }

  void MeshWriter::WriteHeaders(FileReference file, ModelConstRef asset)
  {
    file.write(
      reinterpret_cast<char const*>(&asset.header),
      sizeof(asset.header)
    );

    if (asset.header.meshCount > 0)
    {
	    file.write(
	      reinterpret_cast<char const*>(asset.meshHeaders.data()),
	      sizeof(MeshDataHeader) * asset.header.meshCount
	    );
    }

    if (asset.header.animCount > 0)
    {
	    file.write(
	      reinterpret_cast<char const*>(asset.animHeaders.data()),
	      sizeof(AnimDataHeader) * asset.header.animCount
	    );
    }
  }

  void MeshWriter::WriteData(FileReference file, ModelConstRef asset)
  {
    WriteMeshData(file, asset.meshHeaders, asset.meshes);
    WriteAnimData(file, asset.animHeaders, asset.anims);

    if (!asset.rig.nodes.empty())
    {
			WriteRig(file, asset.rig);
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

    WriteHeaders(file, asset);
    WriteData(file, asset);

    file.close();
  }
}
