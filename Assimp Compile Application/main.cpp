#include "Types/SHAnimationAsset.h"
#include "Types/SHMeshAsset.h"

#include "Libraries/SHAssimpLibrary.h"
#include "Libraries/SHMeshCompiler.h"

#include <vector>
#include <filesystem>

int main(int argc, char* argv[])
{
	SHADE::MeshVector meshes;
	SHADE::AnimVector anims;
	
	std::vector<std::string> paths;

	if (argc == 1)
	{

		for (auto const& dir : std::filesystem::recursive_directory_iterator{"./Assets/"})
		{
			if (dir.path().extension().string() == GLTF_EXTENSION)
			{
				paths.push_back(dir.path().string());
			}
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
		SHADE::SHAssimpLibrary::LoadFromFile(path, meshes, anims);
	}

	for (auto const& mesh : meshes)
	{
		SHADE::SHMeshCompiler::CompileMeshBinary(mesh);
	}

	return 0;
}
