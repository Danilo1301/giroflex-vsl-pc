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

	static void SavePatterns();
	static void LoadPatterns();
	static void SavePattern(Pattern* pattern);
	static Pattern* LoadPattern(std::string fileName);

	static void SaveLightGroups();
	static void LoadLightGroups();
	static void SaveLightGroup(LightGroup* lightGroup);
	static LightGroup* LoadLightGroup(std::string fileName, int modelId);

	static void LoadLocalizations();
	static void LoadLocalization(std::string key);

	static void SaveSettings();
	static void LoadSettings();

};