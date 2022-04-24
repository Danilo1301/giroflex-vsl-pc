#pragma once

#include "plugin.h"

#include "map"

class LocalizationLine {
public:
	std::string key;
	std::map<std::string, std::string> languages;

	LocalizationLine(std::string key) {
		this->key = key;
	}
};

class Localization {
public:
	static std::map<std::string, LocalizationLine*> m_Lines;
	static std::string m_CurrentLanguage;
	static std::vector<std::string> m_AvailableLanguages;

	static void RegisterLine(std::string key, std::string language, std::string text);
	static std::string GetLine(std::string key, std::string language);
	static std::string GetLine(std::string key);
	static std::string GetLineFormatted(std::string key, std::string value);
	static std::string GetLineFormatted(std::string key, int value);
	static std::string GetLineFormatted(std::string key, std::string value1, int value2);
	static void PrintString(std::string key, int time);

	static void RemoveAllLines();
};