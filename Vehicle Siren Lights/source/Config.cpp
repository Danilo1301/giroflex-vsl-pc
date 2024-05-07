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

	CreatePath(GetFullPluginPath(m_DataPath));
	CreatePath(GetFullPluginPath(m_DataPath + m_PatternsPath));
	CreatePath(GetFullPluginPath(m_DataPath + m_VehiclesPath));
	CreatePath(GetFullPluginPath(m_DataPath + m_LocalizationPath));

	DeleteAllConfig();

	SaveSettings();

	SaveLightGroups();

	SavePatterns();
}

void Config::LoadJSON() {
	Log::file << "[Config] * Loading config..." << std::endl;

	CreatePath(GetFullPluginPath(m_DataPath));
	CreatePath(GetFullPluginPath(m_DataPath + m_PatternsPath));
	CreatePath(GetFullPluginPath(m_DataPath + m_VehiclesPath));
	CreatePath(GetFullPluginPath(m_DataPath + m_LocalizationPath));

	LoadSettings();

	LoadPatterns();
	
	LoadLightGroups();

	LoadLocalizations();
}

std::string Config::GetFullPluginPath(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return finalPath;
}

void Config::CreatePath(std::string path) {
	if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
		std::filesystem::create_directory(path);
	}
}

bool Config::Exists(std::string path) {
	return std::filesystem::exists(path);
}

void Config::WriteToFile(std::string path, Json::Value value) {
	//Log::file << "[Config] WriteToFile " << path << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(path);
	file << strJson;
	file.close();
}

Json::Value Config::ReadFile(std::string path) {
	//Log::file << "[Config] ReadFile " << path << std::endl;

	std::ifstream file(path);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), "Vehicle Siren Lights", MB_ICONERROR);
	}

	return value;
}

bool Config::IsPathInsidePluginFolder(std::string path, std::string testFolder)
{
	std::string str1 = ToLower(path);
	std::string str2 = ToLower("data");
	std::string str3 = ToLower(testFolder);

	if (str1.find(str2) != std::string::npos)
	{
		if (testFolder.length() > 0)
		{
			if (str1.find(str3) != std::string::npos)
			{
				return true;
			}
		}
	}
	return false;
}

void Config::DeleteAllConfig() {
	Log::file << "[Config] DeleteAllConfig" << std::endl;

	std::filesystem::remove_all(m_DataPath + "settings.json");

	// Remove /data/vehicles/

	std::vector<std::filesystem::directory_entry> entries;
	for (auto pair : LightGroups::m_LightGroups)
	{
		auto lightGroups = pair.second;
		for (auto lightGroup : lightGroups)
		{
			if (std::find(entries.begin(), entries.end(), lightGroup->modelDirectoryEntry) == entries.end())
			{
				entries.push_back(lightGroup->modelDirectoryEntry);
			}
		}
	}

	for (auto entry : entries)
	{
		Log::file << "[Config] Deleting lightGroups for entry " << entry.path().string() << "..." << std::endl;

		if (!Exists(entry.path().string()))
		{
			Log::file << "[Config] Entry " << entry.path().string() << " does not exist yet, ignoring..." << std::endl;
			continue;
		}

		std::string pathLightGroups = entry.path().string() + m_LightGroupsPath;
		for (const auto& entry2 : std::filesystem::directory_iterator(pathLightGroups))
		{
			std::filesystem::remove_all(entry2.path());
		}

		std::filesystem::remove_all(pathLightGroups);
		std::filesystem::remove_all(entry.path().string() + "\\.vsl-vehicle-config");

		if (IsPathInsidePluginFolder(entry.path().string(), "vehicles"))
		{
			Log::file << "[Config] Deleting root folder..." << std::endl;

			std::filesystem::remove_all(entry.path());
		}
	}

	// Remove /data/patterns/

	std::vector<std::filesystem::directory_entry> patternsEntries;
	for (auto pattern : Patterns::m_Patterns)
	{
		if (std::find(patternsEntries.begin(), patternsEntries.end(), pattern->directoryEntry) == patternsEntries.end())
		{
			patternsEntries.push_back(pattern->directoryEntry);
		}
	}

	for (auto entry : patternsEntries)
	{
		Log::file << "[Config] Deleting patterns .json for entry " << entry.path().string() << "..." << std::endl;

		for (const auto& fileEntry : std::filesystem::directory_iterator(entry))
		{
			if (fileEntry.path().filename().string().find(".json") == std::string::npos)
				continue;

			std::filesystem::remove_all(fileEntry.path());
		}
	}
}

void Config::SavePatterns()
{
	Log::file << "[Config] Saving patterns..." << std::endl;

	std::ofstream file(GetFullPluginPath(m_DataPath + m_PatternsPath + "\\.vsl-patterns-config"));
	file << "";
	file.close();

	for (size_t i = 0; i < Patterns::m_Patterns.size(); i++)
	{
		SavePattern(Patterns::m_Patterns[i]);
	}
}

void Config::LoadPatterns()
{
	Log::file << "[Config] Loading patterns..." << std::endl;

	std::vector<std::filesystem::directory_entry> patternsEntries;

	std::string patternsPath = GetFullPluginPath(m_DataPath + m_PatternsPath);
	auto patternsEntry = std::filesystem::directory_entry(std::filesystem::path(patternsPath));
	patternsEntries.push_back(patternsEntry);

	auto modloaderFolders = SearchModloaderForFile(".vsl-patterns-config");
	for (auto folder : modloaderFolders)
	{
		Log::file << "Found folder on modloader: " << folder.path().string() << std::endl;
		patternsEntries.push_back(folder);
	}

	for (auto patternsEntry : patternsEntries)
	{
		Log::file << "[Config] Loading patterns from " << patternsEntry.path().string() << "..." << std::endl;

		for (const auto& fileEntry : std::filesystem::directory_iterator(patternsEntry.path().string()))
		{
			std::string fileName = fileEntry.path().filename().replace_extension().string();

			if (fileEntry.path().filename().string().find(".json") == std::string::npos)
				continue;

			LoadPattern(fileEntry, patternsEntry);
		}
	}

	/*
	std::string pathPatterns = GetFullPluginPath(m_DataPath + m_PatternsPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathPatterns)) {
		std::string fileName = entry.path().filename().replace_extension().string();

		if (entry.path().filename().string().find(".json") == std::string::npos)
			continue;

		LoadPattern(fileName);
	}
	*/
}

void Config::SavePattern(Pattern* pattern)
{
	Log::file << "[Config] Saving pattern " << pattern->fileName << "..." << std::endl;

	auto path = pattern->directoryEntry.path().string();

	Json::Value value = pattern->ToJSON();
	WriteToFile(path + "\\" + pattern->fileName + ".json", value);
}

Pattern* Config::LoadPattern(std::filesystem::directory_entry fileEntry, std::filesystem::directory_entry patternsEntry)
{
	auto fileName = fileEntry.path().filename().replace_extension().string();
	auto name = fileEntry.path().filename().replace_extension().string();

	Log::file << "[Config] Loading pattern: " << fileName << std::endl;

	if (Patterns::GetPatternByFileName(fileName) != NULL)
	{
		Log::file << "[Config] WARNING: There's already a pattern with the fileName: " << fileName << ", ignoring!" << std::endl;
		return NULL;
	}

	Json::Value value = ReadFile(fileEntry.path().string());

	Pattern* pattern = Patterns::CreatePattern(name);
	pattern->fileName = fileName;
	pattern->directoryEntry = patternsEntry;
	pattern->FromJSON(value);

	return pattern;
}

void Config::SaveLightGroups()
{
	Log::file << "[Config] Saving lightGroups..." << std::endl;

	for (auto pair : LightGroups::m_LightGroups)
	{
		auto modelId = pair.first;
		auto lightGroups = pair.second;

		Log::file << "[Config] Saving ID " << modelId << std::endl;

		for (size_t i = 0; i < lightGroups.size(); i++)
		{
			SaveLightGroup(lightGroups[i]);
		}
	}
}

void Config::LoadLightGroups()
{
	Log::file << "[Config] Loading lightGroups..." << std::endl;

	std::vector<std::filesystem::directory_entry> modelsEntries;

	std::string pathVehicles = GetFullPluginPath(m_DataPath + m_VehiclesPath);
	for (const auto& entry : std::filesystem::directory_iterator(pathVehicles))
	{
		if (!std::filesystem::is_directory(entry.path()))
			continue;

		modelsEntries.push_back(entry);
	}

	auto modloaderFolders = SearchModloaderForFile(".vsl-vehicle-config");
	for (auto folder : modloaderFolders)
	{
		Log::file << "Found folder on modloader: " << folder.path().string() << std::endl;
		modelsEntries.push_back(folder);
	}

	for (auto modelEntry : modelsEntries)
	{
		auto modelId = modelEntry.path().filename().replace_extension().string();

		Log::file << "[Config] Loading vehicle ID " << modelId << "..." << std::endl;

		std::string pathLightGroups = modelEntry.path().string() + m_LightGroupsPath;
		for (const auto& fileEntry : std::filesystem::directory_iterator(pathLightGroups))
		{
			LoadLightGroup(fileEntry, modelEntry);
		}
	}
}

void Config::SaveLightGroup(LightGroup* lightGroup) {
	Log::file << "[Config] Saving lightGroup: " << lightGroup->fileName << std::endl;

	auto path = lightGroup->modelDirectoryEntry.path().string();

	CreatePath(path);
	CreatePath(path + m_LightGroupsPath);

	std::ofstream file(path + "\\.vsl-vehicle-config");
	file << "";
	file.close();

	Json::Value value = lightGroup->ToJSON();
	WriteToFile(path + m_LightGroupsPath + lightGroup->fileName + ".json", value);
}

LightGroup* Config::LoadLightGroup(std::filesystem::directory_entry fileEntry, std::filesystem::directory_entry modelEntry)
{
	auto fileName = fileEntry.path().filename().replace_extension().string();
	int modelId = std::stoi(modelEntry.path().filename().replace_extension());

	Log::file << "[Config] Loading lightGroup for ID " << modelId << std::endl;
	Log::file << "[Config] fileName: '" << fileName << "'" << std::endl;
	Log::file << "[Config] json: '" << fileEntry.path().string() << "'" << std::endl;

	auto otherLightGroups = LightGroups::GetLightGroups(modelId);
	if (otherLightGroups.size() > 0)
	{
		if (otherLightGroups[0]->modelDirectoryEntry.path().string() != modelEntry.path().string())
		{
			Log::file << "[Config] WARNING: This lightGroup has a different modelDirectoryEntry, ignoring!" << std::endl;
			return NULL;
		}
	}

	Json::Value value = ReadFile(fileEntry.path().string());

	LightGroup* lightGroup = LightGroups::CreateLightGroup(modelId, fileName);
	lightGroup->modelDirectoryEntry = modelEntry;
	lightGroup->FromJSON(value);

	Log::file << "[Config] modelDirectoryEntry set to: '" << lightGroup->modelDirectoryEntry.path().string() << "'" << std::endl;

	TestHelper::AddLine("Load lightgroup, fileName set to " + fileName);

	return lightGroup;
}

void Config::LoadLocalizations()
{
	Localization::RemoveAllLines();
	Localization::m_AvailableLanguages.clear();

	std::string pathLocalization = GetFullPluginPath(m_DataPath + m_LocalizationPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathLocalization)) {
		std::string language = entry.path().filename().replace_extension().string();

		Log::file << "[Config] Localization " << language << std::endl;

		Localization::m_AvailableLanguages.push_back(language);
		LoadLocalization(language);
	}
}

void Config::LoadLocalization(std::string key) {
	Json::Value value = ReadFile(GetFullPluginPath(m_DataPath + m_LocalizationPath + key + ".json"));

	for (auto member : value.getMemberNames()) {
		Localization::RegisterLine(member, key, value[member].asString());
	}
}

void Config::SaveSettings() {
	Log::file << "[Config] Saving settings..." << std::endl;

	auto path = GetFullPluginPath(m_DataPath + "settings.json");

	CreatePath(m_DataPath);

	Json::Value value = Json::objectValue;

	value["material_ambient"] = Vehicle::m_MatAmbient;
	value["light_id_offset"] = Vehicle::m_LightIdOffset;

	value["keybinds"] = Keybinds::ToJSON();
	value["language"] = Localization::m_CurrentLanguage;

	WriteToFile(path, value);
}

void Config::LoadSettings() {
	Log::file << "[Config] Loading settings.json" << std::endl;

	auto path = GetFullPluginPath(m_DataPath + "settings.json");

	if (!Exists(path)) {
		Log::file << "[Config] settings not found.json";
		return;
	}

	Json::Value value = ReadFile(path);

	Vehicle::m_MatAmbient = ValidateValue(value["material_ambient"], Vehicle::m_MatAmbient).asFloat();
	Vehicle::m_LightIdOffset = ValidateValue(value["light_id_offset"], Vehicle::m_LightIdOffset).asInt();
	Keybinds::FromJSON(value["keybinds"]);
	if (!value["language"].isNull()) Localization::m_CurrentLanguage = value["language"].asString();

	Log::file << "[Keybind] openMenu: " << Keybinds::openMenu.GetKeybindString() << std::endl;
	Log::file << "[Keybind] reloadConfig: " << Keybinds::reloadConfig.GetKeybindString() << std::endl;
	Log::file << "[Keybind] toggleDebug: " << Keybinds::toggleDebug.GetKeybindString() << std::endl;
	Log::file << "[Keybind] editorSlower: " << Keybinds::editorSlower.GetKeybindString() << std::endl;
}

std::vector<std::filesystem::directory_entry> Config::SearchModloaderForFile(std::string fileName)
{
	Log::file << "[Config] SearchModloaderForFile " << fileName << std::endl;

	std::string path = GAME_PATH("\\modloader");

	std::filesystem::path filePath(path);
	std::filesystem::directory_entry entry(filePath);

	return SearchFolderForFile(entry, fileName);
}

std::vector<std::filesystem::directory_entry> Config::SearchFolderForFile(std::filesystem::directory_entry path, std::string searchFileName)
{
	Log::file << "[Config] Searching path: " << path << std::endl;

	std::vector<std::filesystem::directory_entry> entries;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (std::filesystem::is_regular_file(entry.path()))
		{
			std::string fileName = entry.path().filename().string();

			if (fileName == searchFileName)
			{
				Log::file << "[Config] Found " << fileName << std::endl;

				entries.push_back(path);

				break;
			}
		}
		else if (std::filesystem::is_directory(entry.path()))
		{
			if (IsPathInsidePluginFolder(entry.path().string(), "vehicles") || IsPathInsidePluginFolder(entry.path().string(), "patterns"))
			{
				Log::file << "[Config] Ignoring this folder " << entry.path().string() << std::endl;
				continue;
			}

			auto result = SearchFolderForFile(entry, searchFileName);
			for (auto f : result) entries.push_back(f);
		}
		else {}
	}

	return entries;
}