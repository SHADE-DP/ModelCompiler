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

  void MeshWriter::WriteRig(FileReference file, RigData const& data)
  {
    WriteRigHeader(file, data.header);
    RigWriteNode* root {nullptr};
    WriteRigNodeData(file, data,root);
    WriteRigTree(file, root);
  }

  void MeshWriter::WriteRigHeader(FileReference file, RigDataHeader const& header)
  {
    file.write(
      reinterpret_cast<char const*>(&header.nodeCount),
      sizeof(uint32_t)
    );

    file.write(
      reinterpret_cast<char const*>(header.charCounts.data()),
      sizeof(uint32_t) * header.nodeCount
    );
  }

  void MeshWriter::WriteRigNodeData(FileReference file, RigData const& rig, RigWriteNode*& treeRoot)
  {
    // Build node collection and assign ID to each node BFS STYLE
    // Build tree of nodes using ID

    std::vector<RigNodeDataWrite> dataToWrite;
    dataToWrite.reserve(rig.header.nodeCount);

    std::queue<std::pair<RigWriteNode*, RigNodeData*>> nodeQueue;
    treeRoot = new RigWriteNode;
    treeRoot->id = 0;
    treeRoot->children.clear();
    nodeQueue.emplace(std::make_pair(treeRoot, rig.root));
    dataToWrite.emplace_back(rig.root->name, rig.root->transform);

    while(!nodeQueue.empty())
    {
	    auto currPair = nodeQueue.front();
      nodeQueue.pop();
      auto currWriteNode = currPair.first;
      auto currDataNode = currPair.second;

      for (auto child : currDataNode->children)
      {
	      auto newPair = std::make_pair(new RigWriteNode(), child);
        newPair.first->id = dataToWrite.size();
        currWriteNode->children.push_back(newPair.first);
        nodeQueue.push(newPair);

        dataToWrite.emplace_back(child->name, child->transform);
      }

      delete currDataNode;
    }

    for (auto const& data : dataToWrite)
    {
	    file.write(data.name.c_str(), data.name.size());
      file.write(
				reinterpret_cast<char const*>(&data.transform),
        sizeof(SHMat4)
      );
    }
  }

  void MeshWriter::WriteRigTree(FileReference file, RigWriteNode const* root)
  {
    std::queue<RigWriteNode const*> nodeQueue;
    nodeQueue.push(root);

    while(!nodeQueue.empty())
    {
      auto node = nodeQueue.front();
      nodeQueue.pop();

	    file.write(
				reinterpret_cast<char const*>(&node->id),
        sizeof(uint32_t)
      );

      uint32_t size = node->children.size();
      
	    file.write(
				reinterpret_cast<char const*>(&size),
        sizeof(uint32_t)
      );

      for (auto child : node->children)
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
    WriteRig(file, asset.rig);

    file.close();
  }
}
