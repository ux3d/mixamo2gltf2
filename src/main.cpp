#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct DecomposedPath {
	std::string parentPath = "";
	std::string stem = "";
	std::string extension = "";
};

void decomposePath(DecomposedPath& decomposedPath, const std::string& path)
{
	std::filesystem::path filesystemPath(path);

	decomposedPath.parentPath = filesystemPath.parent_path().generic_string();
	decomposedPath.stem = filesystemPath.stem().generic_string();
	decomposedPath.extension = filesystemPath.extension().generic_string();
}

bool loadFile(std::string& output, const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	file.seekg(0);

	output.resize(fileSize);

	file.read(output.data(), fileSize);
	file.close();

	return true;
}

bool saveFile(const std::string& output, const std::string& filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write(output.data(), output.size());
	file.close();

	return true;
}

bool copyUriFile(const std::string& uri, const DecomposedPath& saveDecomposedPath, const DecomposedPath& mainDecomposedPath)
{
	if (uri.find("data") != 0)
	{
    	std::string binaryPath;
    	DecomposedPath binaryDecomposedPath;
		std::string binaryData;

		decomposePath(binaryDecomposedPath, uri);
		binaryDecomposedPath.parentPath = saveDecomposedPath.parentPath;

		binaryPath = mainDecomposedPath.parentPath + "/" + uri;
		if (!loadFile(binaryData, binaryPath))
		{
			printf("Error: Could not load uri file '%s'\n", binaryPath.c_str());

			return false;
		}

		binaryPath = binaryDecomposedPath.parentPath + "/" + uri;
		if (!saveFile(binaryData, binaryPath))
		{
			printf("Error: Could not save uri file '%s' \n", binaryPath.c_str());

			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[])
{
	if (argc <= 3)
	{
		printf("Usage: mixamo2gltf2 folder mainGltf animationGltF\n");

		return 0;
	}

	// Decomposing paths

	DecomposedPath saveDecomposedPath;
	saveDecomposedPath.parentPath = argv[1];

	DecomposedPath mainDecomposedPath;
	std::string mainName = argv[2];
	decomposePath(mainDecomposedPath, mainName);

	DecomposedPath animationDecomposedPath;
	std::string animationName = argv[3];
	decomposePath(animationDecomposedPath, animationName);

	// Loading glTF

	std::string mainData;
	if (!loadFile(mainData, mainName))
	{
		printf("Error: Could not load glTF '%s'\n", mainName.c_str());

		return -1;
	}

	std::string animationData;
	if (!loadFile(animationData, animationName))
	{
		printf("Error: Could not load glTF '%s'\n", animationName.c_str());

		return -1;
	}

	// Parsing glTF

    json mainGltf = json::parse(mainData);
    json animationGltf = json::parse(animationData);

    json saveGltf = mainGltf;

    //
    // Processing main glTF
    //

    // Images

    if (mainGltf.contains("images"))
    {
    	for (size_t i = 0; i < mainGltf["images"].size(); i++)
    	{
    		if (mainGltf["images"][i].contains("uri"))
    		{
    			std::string uri = mainGltf["images"][i]["uri"].get<std::string>();

				if (!copyUriFile(uri, saveDecomposedPath, mainDecomposedPath))
				{
					printf("Error: Could not copy image data '%s'\n", uri.c_str());

					return -1;
				}
    		}
    	}
    }

    // Buffers

    size_t mainBuffersSize = 0;

    if (mainGltf.contains("buffers"))
    {
    	mainBuffersSize = mainGltf["buffers"].size();

    	for (size_t i = 0; i < mainGltf["buffers"].size(); i++)
    	{
    		if (mainGltf["buffers"][i].contains("uri"))
    		{
    			std::string uri = mainGltf["buffers"][i]["uri"].get<std::string>();

				if (!copyUriFile(uri, saveDecomposedPath, mainDecomposedPath))
				{
					printf("Error: Could not copy binary data '%s'\n", uri.c_str());

					return -1;
				}
    		}
    	}
    }

    // BufferViews

    size_t mainBufferViewsSize = 0;
    if (mainGltf.contains("bufferViews"))
    {
    	mainBufferViewsSize = mainGltf["bufferViews"].size();
    }

    // Accessors

    size_t mainAccessorsSize = 0;
    if (mainGltf.contains("accessors"))
    {
    	mainAccessorsSize = mainGltf["accessors"].size();
    }

    //
    // Processing animation glTF
    //

    // Buffers

    if (animationGltf.contains("buffers"))
    {
    	for (size_t i = 0; i < animationGltf["buffers"].size(); i++)
    	{
    		if (animationGltf["buffers"][i].contains("uri"))
    		{
    			std::string uri = animationGltf["buffers"][i]["uri"].get<std::string>();

				if (!copyUriFile(uri, saveDecomposedPath, animationDecomposedPath))
				{
					printf("Error: Could not copy binary data '%s'\n", uri.c_str());

					return -1;
				}
    		}

    		//

    		saveGltf["buffers"].push_back(animationGltf["buffers"][i]);
    	}
    }

    // BufferViews

    if (animationGltf.contains("bufferViews"))
    {
    	for (size_t i = 0; i < animationGltf["bufferViews"].size(); i++)
    	{
    		json bufferView = animationGltf["bufferViews"][i];
    		bufferView["buffer"] = bufferView["buffer"].get<size_t>() + mainBuffersSize;

    		//

    		saveGltf["bufferViews"].push_back(bufferView);
    	}
    }

    // Accessors

    if (animationGltf.contains("accessors"))
    {
    	for (size_t i = 0; i < animationGltf["accessors"].size(); i++)
    	{
    		json accessor = animationGltf["accessors"][i];
    		accessor["bufferView"] = accessor["bufferView"].get<size_t>() + mainBufferViewsSize;

    		//

    		saveGltf["accessors"].push_back(accessor);
    	}
    }

    // Animations

    if (animationGltf.contains("animations"))
    {
    	json animations = animationGltf["animations"];

    	for (size_t i = 0; i < animations.size(); i++)
    	{
    	    if (animations[i].contains("samplers"))
    	    {
    	    	for (size_t k = 0; k < animations[i]["samplers"].size(); k++)
    	    	{
    	    		animations[i]["samplers"][k]["input"] = animations[i]["samplers"][k]["input"].get<size_t>() + mainAccessorsSize;
    	    		animations[i]["samplers"][k]["output"] = animations[i]["samplers"][k]["output"].get<size_t>() + mainAccessorsSize;
    	    	}
    	    }

    	    //

    	    saveGltf["animations"].push_back(animations[i]);
    	}
    }

    //
    // Save merged animations glTF
    //

	saveDecomposedPath.stem = "merged";
	saveDecomposedPath.extension = ".gltf";
    std::string saveName = saveDecomposedPath.parentPath + "/" + saveDecomposedPath.stem  + saveDecomposedPath.extension;

	if (!saveFile(saveGltf.dump(3), saveName))
	{
		printf("Error: Could not save merged glTF file '%s' \n", saveName.c_str());

		return -1;
	}

	printf("Success: Saved merged glTF file '%s'\n", saveName.c_str());

	return 0;
}
