#include "Localization.h"

#include "../Log.h"

#include "CMessages.h"

std::map<std::string, LocalizationLine*> Localization::m_Lines;
std::string Localization::m_CurrentLanguage = "en";
std::vector<std::string> Localization::m_AvailableLanguages;

void Localization::RegisterLine(std::string key, std::string language, std::string text) {
	Log::file << "[Localization] RegisterLine " << key << " (" << language << ") " << "'" << text << "'" << std::endl;

	if (m_Lines.count(key) == 0)
	{
		m_Lines[key] = new LocalizationLine(key);
	}

	auto line = m_Lines[key];
	line->languages[language] = text;
}

std::string Localization::GetLine(std::string key, std::string language) {
	if (m_Lines.count(key) > 0)
	{
		auto line = m_Lines[key];

		if (line->languages.count(language) > 0)
		{
			//return "[" + language + "]" + line->languages[language];
			return line->languages[language];
		}
	}

	return key + "_" + language;
}

std::string Localization::GetLine(std::string key) {
	return GetLine(key, m_CurrentLanguage);
}


std::string Localization::GetLineFormatted(std::string key, std::string value) {
	char text[256];
	sprintf(text, GetLine(key).c_str(), value.c_str());
	return std::string(text);
}

std::string Localization::GetLineFormatted(std::string key, int value) {
	char text[256];
	sprintf(text, GetLine(key).c_str(), value);
	return text;
}

std::string Localization::GetLineFormatted(std::string key, std::string value1, int value2) {
	char text[256];
	sprintf(text, GetLine(key).c_str(), value1.c_str(), value2);
	return text;
}

void Localization::PrintString(std::string key, int time) {
	static char message[256];
	snprintf(message, 256, "%s", GetLine(key).c_str());
	CMessages::AddMessageJumpQ(message, time, 0, false);
}

void Localization::RemoveAllLines() {
	std::vector<LocalizationLine*> toremove;
	for (auto pair : m_Lines) toremove.push_back(pair.second);

	for (auto a : toremove) {
		delete a;
	}

	m_Lines.clear();
}