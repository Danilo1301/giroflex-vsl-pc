#pragma once

#include "plugin.h"
#include "map"

class Language {
public:
	//static std::ifstream langconfig_file;
	static std::map<std::string, std::string> m_Texts;

	//static void LoadConfig();
	static const char* GetText(const char* line);
	static const char* GetIntFormatText(const char* line, const char* format, int num);
	static void PrintText(const char* line, int time);
};