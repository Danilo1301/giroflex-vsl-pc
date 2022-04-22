#pragma once

#include "pch.h"
#include "Patterns.h"
#include "LightGroups.h"

class Config {
public:
	static std::string m_DataPath;
	static std::string m_VehiclesPath;
	static std::string m_PatternsPath;
	static std::string m_LightGroupsPath;

	static void SaveJSON();
	static void LoadJSON();
	static std::string GetFullPath(std::string path);
	static void CreatePath(std::string path);
	static void WriteToFile(std::string path, Json::Value value);
	static Json::Value ReadFile(std::string path);

	static void SavePattern(int index, Pattern* pattern);
	static Pattern* LoadPattern(int index);

	static void SaveLightGroup(int index, LightGroup* lightGroup);
	static LightGroup* LoadLightGroup(int index, int modelId);
};