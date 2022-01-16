#include "JsonHelper.h"
#include <fstream>
#include <filesystem>

nlohmann::json JsonHelper::loadJsonFromFile(const std::filesystem::path& path)
{
	nlohmann::json root;
	
	std::ifstream jsonFile(path);
	jsonFile >> root;
	
	return root;
}

void JsonHelper::saveJsonToFile(const nlohmann::json& json, const std::filesystem::path& path, bool beautify)
{
	std::ofstream jsonFile(path);
	if (beautify)
	{
		jsonFile << std::setw(4);
	}
	jsonFile << json;
}
