#pragma once

#include "pch.h"

class Config {
public:
	static std::string GetFullPath(std::string path);
	static void CreatePath(std::string path);
	static bool Exists(std::string path);
	static void WriteToFile(std::string path, Json::Value value);
	static Json::Value ReadFile(std::string path);

	static void Save();
	static void DeleteAllConfig();
	static void Load();
};