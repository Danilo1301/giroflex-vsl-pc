#pragma once

#include "Window.h"

enum EDIT_TYPE {
	EDIT_STRING,
	EDIT_FLOAT,
	EDIT_INT,
	EDIT_UCHAR
};

class TextEditor {
public:
	static bool m_Visible;
	static std::string m_Title;

	static EDIT_TYPE m_EditType;

	static bool m_AutoUpdate;

	static std::string m_Value;

	static std::string* m_pStr;
	static int* m_pInt;
	static float* m_pFloat;
	static unsigned char* m_pUChar;

	static CVector2D m_Size;

	static std::function<void(void)> m_OnConfirm;

	static std::string m_AvailableChars;
	static std::string m_Numbers;

	

	static void Update();
	static void Draw();
	static void Open(std::string title, bool autoUpdate, std::string* value);
	static void Open(std::string title, bool autoUpdate, int* value);
	static void Open(std::string title, bool autoUpdate, float* value);
	static void Open(std::string title, bool autoUpdate, unsigned char* value);
	static void Close();
	static void UpdateValue();
};