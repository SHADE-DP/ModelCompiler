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

#include "Libraries/MeshCompiler.h"

#include <vector>
#include <filesystem>

int main(int argc, char* argv[])
{	
	std::vector<std::string> paths;
	
	#if 1

	if (argc == 1)
	{
		#if 1
		if (std::filesystem::is_directory(ASSET_ROOT))
		{
			for (auto& dir : 
				std::filesystem::recursive_directory_iterator{ ASSET_ROOT })
			{
				if (dir.path().extension().string() == GLTF_EXTENSION || 
					dir.path().extension().string() == FBX_EXTENSION)
				{
					auto path = dir.path();
					path.make_preferred();
					paths.push_back(path.string());
				}
			}
		}
		else
		{
			std::cout << "Default path not found!" << std::endl;
			return 1;
		}
		#else
		for (auto& dir : std::filesystem::directory_iterator{ "./" })
		{
				if (dir.path().extension().string() == GLTF_EXTENSION || 
					dir.path().extension().string() == FBX_EXTENSION)
				{
					auto path = dir.path();
					path.make_preferred();
					paths.push_back(path.string());
				}
		}
		#endif
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
		SH_COMP::MeshCompiler::LoadAndCompile(path);
		std::cout << "[Mesh Compiler] Compiled file: " << path << std::endl;
	}
	
	#else
	(void)argc;
	(void)argv;
	SH_COMP::MeshCompiler::LoadAndCompile("MD_HomeownerV2.gltf");
	#endif

	return 0;
}
