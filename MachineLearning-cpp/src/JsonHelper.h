#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>

class JsonHelper
{
public:
	static nlohmann::json loadJsonFromFile(const std::filesystem::path& path);
	static void saveJsonToFile(const nlohmann::json& json, const std::filesystem::path& path, bool beautify = false);
};