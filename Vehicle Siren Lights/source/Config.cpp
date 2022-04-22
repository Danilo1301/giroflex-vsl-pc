#include "Config.h"
#include <filesystem>

std::string Config::m_DataPath = "\\data\\";
std::string Config::m_VehiclesPath = "\\vehicles\\";
std::string Config::m_PatternsPath = "\\patterns\\";
std::string Config::m_LightGroupsPath = "\\lightgroups\\";

void Config::SaveJSON() {
	Log::file << "[Config] Saving config" << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);
	CreatePath(m_DataPath + m_VehiclesPath);

	DeleteAllConfig();

	for (size_t i = 0; i < Patterns::m_Patterns.size(); i++)
	{
		SavePattern(i, Patterns::m_Patterns[i]);
	}

	for (auto pair : LightGroups::m_LightGroups)
	{
		auto modelId = pair.first;
		auto lightGroups = pair.second;

		for (size_t i = 0; i < lightGroups.size(); i++)
		{
			SaveLightGroup(i, lightGroups[i]);
		}
	}
}

void Config::LoadJSON() {
	Log::file << "[Config] Loading config" << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);
	CreatePath(m_DataPath + m_VehiclesPath);

	std::string pathPatterns = GetFullPath(m_DataPath + m_PatternsPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathPatterns)) {
		int index = std::stoi(entry.path().filename().replace_extension());

		Log::file << "[Config] Pattern " << index << std::endl;

		LoadPattern(index);
	}
	
	//

	std::string pathVehicles = GetFullPath(m_DataPath + m_VehiclesPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathVehicles)) {
		int modelId = std::stoi(entry.path().filename().replace_extension());

		Log::file << "[Config] Vehicle " << modelId << std::endl;

		std::string pathLightGroups = GetFullPath(m_DataPath + m_VehiclesPath + std::to_string(modelId) + m_LightGroupsPath);

		for (const auto& entry2 : std::filesystem::directory_iterator(pathLightGroups)) {
			int index = std::stoi(entry2.path().filename().replace_extension());

			Log::file << "[Config] LightGroup " << index << std::endl;

			LoadLightGroup(index, modelId);
		}
	}
}

std::string Config::GetFullPath(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return finalPath;
}

void Config::CreatePath(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	if (!std::filesystem::is_directory(finalPath) || !std::filesystem::exists(finalPath)) {
		Log::file << "[Config] CreatePath " << finalPath << std::endl;

		std::filesystem::create_directory(finalPath);
	}
}

void Config::WriteToFile(std::string path, Json::Value value) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	Log::file << "[Config] WriteToFile " << finalPath << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(finalPath);
	file << strJson;
	file.close();
}

Json::Value Config::ReadFile(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	Log::file << "[Config] ReadFile " << finalPath << std::endl;

	std::ifstream file(finalPath);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), "Vehicle Siren Lights", MB_ICONERROR);
	}

	return value;
}

void Config::DeleteAllConfig() {
	Log::file << "[Config] DeleteAllConfig" << std::endl;

	std::string pathPatterns = GetFullPath(m_DataPath + m_PatternsPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathPatterns)) {
		std::filesystem::remove_all(entry.path());
	}

	//

	std::string pathVehicles = GetFullPath(m_DataPath + m_VehiclesPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathVehicles)) {
		int modelId = std::stoi(entry.path().filename().replace_extension());

		std::string pathLightGroups = GetFullPath(m_DataPath + m_VehiclesPath + std::to_string(modelId) + m_LightGroupsPath);

		for (const auto& entry2 : std::filesystem::directory_iterator(pathLightGroups)) {
			std::filesystem::remove_all(entry2.path());
		}

		std::filesystem::remove_all(entry.path());
	}
}

void Config::SavePattern(int index, Pattern* pattern) {
	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);

	Json::Value value = pattern->ToJSON();

	WriteToFile(m_DataPath + m_PatternsPath + std::to_string(index) + ".json", value);
}

Pattern* Config::LoadPattern(int index) {
	Json::Value value = ReadFile(m_DataPath + m_PatternsPath + std::to_string(index) + ".json");

	Pattern* pattern = Patterns::CreatePattern(value["name"].asString());
	pattern->FromJSON(value);

	return pattern;
}

void Config::SaveLightGroup(int index, LightGroup* lightGroup) {
	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_VehiclesPath);
	CreatePath(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId));
	CreatePath(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId) + m_LightGroupsPath);

	Json::Value value = lightGroup->ToJSON();

	WriteToFile(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId) + m_LightGroupsPath + std::to_string(index) + ".json", value);
}

LightGroup* Config::LoadLightGroup(int index, int modelId) {
	Json::Value value = ReadFile(m_DataPath + m_VehiclesPath + std::to_string(modelId) + m_LightGroupsPath + std::to_string(index) + ".json");

	LightGroup* lightGroup = LightGroups::CreateLightGroup(modelId);
	lightGroup->FromJSON(value);

	return lightGroup;
}