#include "Language.h"

#include "CMessages.h"

std::map<std::string, std::string> Language::m_Texts;

const char* Language::GetText(const char* line) {
	if (m_Texts.find(line) == m_Texts.end()) {
		return line;
	}

	return m_Texts.at(line).c_str();
}

const char* Language::GetIntFormatText(const char* line, const char* format, int num) {
	static char message[256];
	snprintf(message, 256, format, line, num);

	if (m_Texts.find(line) != m_Texts.end()) {
		snprintf(message, 256, format, GetText(line), num);
	}

	return message;
}

void Language::PrintText(const char* line, int time) {
	static char message[256];
	snprintf(message, 256, "%s", line);

	if (m_Texts.find(line) != m_Texts.end()) {
		snprintf(message, 256, "%s", GetText(line));
	}

	CMessages::AddMessageJumpQ(message, time, 0, false);
}