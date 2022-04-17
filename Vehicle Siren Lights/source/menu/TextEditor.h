#pragma once

#include "Window.h"

class TextEditor {
public:
	static bool m_Visible;
	static std::string* m_Value;
	static std::string m_Title;

	static CVector2D m_Size;

	static std::function<void(void)> m_OnConfirm;

	static std::string m_AvailableChars;

	static void Update();
	static void Draw();
	static void Open(std::string title, std::string* value);
	static void Close();
};