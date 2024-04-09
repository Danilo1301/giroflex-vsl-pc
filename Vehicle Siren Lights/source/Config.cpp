#include "Config.h"
#include <filesystem>

#include "Vehicle.h"
#include "TestHelper.h"
#include "Keybinds.h"

#include "log/Log.h"
#include "localization/Localization.h"

std::string Config::m_DataPath = "\\data\\";
std::string Config::m_VehiclesPath = "\\vehicles\\";
std::string Config::m_PatternsPath = "\\patterns\\";
std::string Config::m_LocalizationPath = "\\localization\\";
std::string Config::m_LightGroupsPath = "\\lightgroups\\";

void Config::SaveJSON() {
	Log::file << "[Config] * Saving config..." << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);
	CreatePath(m_DataPath + m_VehiclesPath);
	CreatePath(m_DataPath + m_LocalizationPath);

	DeleteAllConfig();

	Log::file << "[Config] Saving settings..." << std::endl;

	SaveSettings();

	for (auto pair : LightGroups::m_LightGroups)
	{
		auto modelId = pair.first;
		auto lightGroups = pair.second;

		for (size_t i = 0; i < lightGroups.size(); i++)
		{
			SaveLightGroup(lightGroups[i]);
		}
	}

	for (size_t i = 0; i < Patterns::m_Patterns.size(); i++)
	{
		SavePattern(Patterns::m_Patterns[i]);
	}
}

void Config::LoadJSON() {
	Log::file << "[Config] * Loading config..." << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);
	CreatePath(m_DataPath + m_VehiclesPath);
	CreatePath(m_DataPath + m_LocalizationPath);

	LoadSettings();

	Log::file << "[Config] Loading patterns..." << std::endl;

	std::string pathPatterns = GetFullPath(m_DataPath + m_PatternsPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathPatterns)) {
		std::string fileName = entry.path().filename().replace_extension().string();

		LoadPattern(fileName);
	}
	
	//

	Log::file << "[Config] Loading vehicles..." << std::endl;

	std::string pathVehicles = GetFullPath(m_DataPath + m_VehiclesPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathVehicles)) {
		int modelId = std::stoi(entry.path().filename().replace_extension());

		Log::file << "[Config] Loading vehicle ID " << modelId  << "..." << std::endl;

		std::string pathLightGroups = GetFullPath(m_DataPath + m_VehiclesPath + std::to_string(modelId) + m_LightGroupsPath);

		for (const auto& entry2 : std::filesystem::directory_iterator(pathLightGroups)) {
			std::string fileName = entry2.path().filename().replace_extension().string();

			LoadLightGroup(fileName, modelId);
		}
	}

	//

	Localization::RemoveAllLines();
	Localization::m_AvailableLanguages.clear();

	std::string pathLocalization = GetFullPath(m_DataPath + m_LocalizationPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathLocalization)) {
		std::string language = entry.path().filename().replace_extension().string();

		Log::file << "[Config] Localization " << language << std::endl;

		Localization::m_AvailableLanguages.push_back(language);
		LoadLocalization(language);
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
		std::filesystem::create_directory(finalPath);
	}
}

bool Config::Exists(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return std::filesystem::exists(finalPath);
}


void Config::WriteToFile(std::string path, Json::Value value) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	//Log::file << "[Config] WriteToFile " << finalPath << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(finalPath);
	file << strJson;
	file.close();
}

Json::Value Config::ReadFile(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	//Log::file << "[Config] ReadFile " << finalPath << std::endl;

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

	std::filesystem::remove_all(m_DataPath + "settings.json");

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

	//

	std::string pathPatterns = GetFullPath(m_DataPath + m_PatternsPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathPatterns)) {
		std::filesystem::remove_all(entry.path());
	}
}

void Config::SavePattern(Pattern* pattern) {
	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);

	Json::Value value = pattern->ToJSON();

	WriteToFile(m_DataPath + m_PatternsPath + pattern->fileName + ".json", value);
}

Pattern* Config::LoadPattern(std::string fileName) {
	Log::file << "[Config] Loading pattern: " << fileName << std::endl;

	Json::Value value = ReadFile(m_DataPath + m_PatternsPath + fileName + ".json");

	Pattern* pattern = Patterns::CreatePattern(value["name"].asString());
	pattern->fileName = fileName;
	pattern->FromJSON(value);

	return pattern;
}

void Config::SaveLightGroup(LightGroup* lightGroup) {
	Log::file << "[Config] Saving lightGroup: " << lightGroup->fileName << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_VehiclesPath);
	CreatePath(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId));
	CreatePath(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId) + m_LightGroupsPath);

	Json::Value value = lightGroup->ToJSON();

	WriteToFile(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId) + m_LightGroupsPath + lightGroup->fileName + ".json", value);
}

LightGroup* Config::LoadLightGroup(std::string fileName, int modelId) {
	Log::file << "[Config] Loading lightGroup: " << fileName << " for ID " << modelId << std::endl;

	Json::Value value = ReadFile(m_DataPath + m_VehiclesPath + std::to_string(modelId) + m_LightGroupsPath + fileName + ".json");

	LightGroup* lightGroup = LightGroups::CreateLightGroup(modelId, fileName);
	lightGroup->FromJSON(value);

	TestHelper::AddLine("Load lightgroup, fileName set to " + fileName);

	return lightGroup;
}

void Config::LoadLocalization(std::string key) {
	Json::Value value = ReadFile(m_DataPath + m_LocalizationPath + key + ".json");

	for (auto member : value.getMemberNames()) {
		Localization::RegisterLine(member, key, value[member].asString());
	}
}

void Config::SaveSettings() {
	auto path = m_DataPath + "settings.json";

	CreatePath(m_DataPath);

	Json::Value value = Json::objectValue;

	value["material_ambient"] = Vehicle::m_MatAmbient;
	value["keybinds"] = Keybinds::ToJSON();
	value["language"] = Localization::m_CurrentLanguage;

	WriteToFile(path, value);
}

void Config::LoadSettings() {
	Log::file << "[Config] Loading settings.json" << std::endl;

	auto path = m_DataPath + "settings.json";

	if (!Exists(path)) {
		Log::file << "[Config] settings not found.json";
		return;
	}

	Json::Value value = ReadFile(path);

	Vehicle::m_MatAmbient = value["material_ambient"].asFloat();
	Keybinds::FromJSON(value["keybinds"]);
	if (!value["language"].isNull()) Localization::m_CurrentLanguage = value["language"].asString();

	Log::file << "[Keybind] openMenu: " << Keybinds::openMenu.GetKeybindString() << std::endl;
	Log::file << "[Keybind] reloadConfig: " << Keybinds::reloadConfig.GetKeybindString() << std::endl;
	Log::file << "[Keybind] toggleDebug: " << Keybinds::toggleDebug.GetKeybindString() << std::endl;
	Log::file << "[Keybind] editorSlower: " << Keybinds::editorSlower.GetKeybindString() << std::endl;
}