/******************************************************************************
 * \file    main.cpp
 * \author  Loh Xiao Qi
 * \date    31 October 2022
 * \brief   
 * 
 * \copyright	Copyright (c) 2021 Digipen Institute of Technology. Reproduction
 *		        or disclosure of this file or its contents without the prior
 *			      written consent of Digipen Institute of Technology is prohibited.
 ******************************************************************************/
#include "Libraries/SHMeshCompiler.h"

#include <vector>
#include <filesystem>

int main(int argc, char* argv[])
{	
	std::vector<std::string> paths;

	if (argc == 1)
	{
		if (std::filesystem::is_directory(ASSET_ROOT))
		{
			for (auto const& dir : std::filesystem::recursive_directory_iterator{ "./Assets/" })
			{
				if (dir.path().extension().string() == GLTF_EXTENSION || 
					dir.path().extension().string() == FBX_EXTENSION)
				{
					paths.push_back(dir.path().string());
				}
			}
		}
		else
		{
			return 1;
		}
	}
	else if (argc > 1)
	{
		for (int i { 1 }; i < argc; ++i)
		{
			paths.emplace_back(argv[i]);
		}
	}

	for (auto const& path : paths)
	{
		SHADE::SHMeshCompiler::LoadAndCompile(path);
	}

	return 0;
}
