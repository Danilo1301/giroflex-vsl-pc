#pragma once

#include "pch.h"

#include "Patterns.h"
#include "LightGroups.h"

class Config {
public:
	static std::string m_DataPath;
	static std::string m_VehiclesPath;
	static std::string m_PatternsPath;
	static std::string m_LocalizationPath;
	static std::string m_LightGroupsPath;

	static void SaveJSON();
	static void DeleteAllConfig();
	static void LoadJSON();
	static std::string GetFullPluginPath(std::string path);
	static void CreatePath(std::string path);
	static bool Exists(std::string path);
	static void WriteToFile(std::string path, Json::Value value);
	static Json::Value ReadFile(std::string path);
	static bool IsPathInsidePluginFolder(std::string path, std::string testFolder);

	static void SavePatterns();
	static void LoadPatterns();
	static void SavePattern(Pattern* pattern);
	static Pattern* LoadPattern(std::filesystem::directory_entry fileEntry, std::filesystem::directory_entry patternsEntry);

	static void SaveLightGroups();
	static void LoadLightGroups();
	static void SaveLightGroup(LightGroup* lightGroup);
	static LightGroup* LoadLightGroup(std::filesystem::directory_entry fileEntry, std::filesystem::directory_entry modelEntry);

	static void LoadLocalizations();
	static void LoadLocalization(std::string key);

	static void SaveSettings();
	static void LoadSettings();

	static std::vector<std::filesystem::directory_entry> SearchModloaderForFile(std::string fileName);
	static std::vector<std::filesystem::directory_entry> SearchFolderForFile(std::filesystem::directory_entry path, std::string searchFileName);
};