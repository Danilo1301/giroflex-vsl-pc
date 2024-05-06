#include "TextEditor.h"
#include "Menu.h"
#include "../input/Input.h"

#include "../TestHelper.h"

#include <iostream>

bool TextEditor::m_Visible = false;
std::string TextEditor::m_Title = "";

EDIT_TYPE TextEditor::m_EditType = EDIT_TYPE::EDIT_STRING;

bool TextEditor::m_AutoUpdate = false;

std::string TextEditor::m_Value = "";

std::string* TextEditor::m_pStr = nullptr;
int* TextEditor::m_pInt = nullptr;
float* TextEditor::m_pFloat = nullptr;
unsigned char* TextEditor::m_pUChar = nullptr;

CVector2D TextEditor::m_Size = CVector2D(500, 120);
std::function<void(void)> TextEditor::m_OnConfirm = NULL;

std::string TextEditor::m_AvailableChars = "בדגהףץפצתûü abcdefghijklmnopqrstuvwxyz0123456789-=+_,.~;:!|()*@#$%&";
std::string TextEditor::m_Numbers = "0123456789";

void TextEditor::Update() {
	//if (m_pStr == nullptr && m_pInt == nullptr) return;
	if (!m_Visible) return;

	for (int i = 0; i < 200; i++) {
		if (Input::GetKeyDown(i)) {
			bool capslockOn = GetKeyState(20) || (GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_RSHIFT) & 0x8000);

			bool shiftDown = ((GetKeyState(VK_SHIFT) & 0x8000) != 0);

			char c = MapVirtualKey(i, MAPVK_VK_TO_CHAR);
			std::string charUpperStr(1, std::toupper(c));
			std::string charLowerStr(1, std::tolower(c));

			auto len = m_Value.length();

			if (i == 8) {
				if(len > 0) m_Value.pop_back();
			}
			else {
				//Log::file << i << ": (" << c << ") (" << charUpperStr << ")" << (capslockOn ? "CAPS" : "NOCAPS") << std::endl;

				TestHelper::AddLine(std::to_string(i) + ": (" + c + ")");

				if (len < 80)
				{
					if (i == 189 && shiftDown) // [ SHIFT + -/_ ]
					{
						m_Value += "_";
					}
					else
					{
						if (m_AvailableChars.find(charLowerStr) != std::string::npos)
						{
							m_Value += capslockOn ? charUpperStr : charLowerStr;
						}
					}
				}
			}

			if (m_AutoUpdate) {
				UpdateValue();
			}
		}
	}
	
	if (Input::GetKeyDown(13)) {
		Close();
	}
}

void TextEditor::Draw() {
	float x = 1024 / 2 - m_Size.x / 2;
	float y = 768 - m_Size.y - 80.0f;

	Menu::DrawRect(x, y, m_Size.x, m_Size.y, CRGBA(0, 0, 0, 190));

	Menu::DrawString(m_Title, x + 10.0f, y + 10.0f, CRGBA(255, 255, 255));

	float inputW = m_Size.x - 20.0f;
	float inputH = 35.0f;
	float inputX = x + 10.0f;
	float inputY = y + 30.0f;

	Menu::DrawRect(inputX, inputY, inputW, inputH, CRGBA(255, 255, 255));

	//if (m_Value == nullptr) return;

	Menu::m_FontAlign = eFontAlignment::ALIGN_CENTER;
	Menu::DrawString("[ " + m_Value + " ]", inputX + inputW / 2, inputY + inputH / 2 - 10.0f, CRGBA(0, 0, 0));
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;
}

void TextEditor::Open(std::string title, bool autoUpdate, std::string* value) {
	m_Title = title;
	m_Value = *value;
	m_pStr = value;

	m_Visible = true;
	m_EditType = EDIT_TYPE::EDIT_STRING;
	m_AutoUpdate = autoUpdate;
}

void TextEditor::Open(std::string title, bool autoUpdate, int* value) {
	m_Title = title;
	m_Value = std::to_string(*value);
	m_pInt = value;

	m_Visible = true;
	m_EditType = EDIT_TYPE::EDIT_INT;
	m_AutoUpdate = autoUpdate;
}

void TextEditor::Open(std::string title, bool autoUpdate, float* value) {
	m_Title = title;
	m_Value = std::to_string(*value);
	m_pFloat = value;

	m_Visible = true;
	m_EditType = EDIT_TYPE::EDIT_FLOAT;
	m_AutoUpdate = autoUpdate;
}

void TextEditor::Open(std::string title, bool autoUpdate, unsigned char* value) {
	m_Title = title;
	m_Value = std::to_string(*value);
	m_pUChar = value;

	m_Visible = true;
	m_EditType = EDIT_TYPE::EDIT_UCHAR;
	m_AutoUpdate = autoUpdate;
}

void TextEditor::Close() {
	if (!m_AutoUpdate) {
		UpdateValue();
	}

	m_pInt = nullptr;
	m_pStr = nullptr;
	m_Value = "";
	m_Visible = false;
}

void TextEditor::UpdateValue() {

	if (m_EditType == EDIT_TYPE::EDIT_STRING)
	{
		*m_pStr = m_Value;
	}

	if (m_EditType == EDIT_TYPE::EDIT_INT)
	{
		*m_pInt = atoi(m_Value.c_str());
	}

	if (m_EditType == EDIT_TYPE::EDIT_UCHAR)
	{
		int value = atoi(m_Value.c_str());
		if (value < 0) value = 0;
		if (value > 255) value = 255;
		*m_pUChar = (unsigned char)value;
	}

	if (m_EditType == EDIT_TYPE::EDIT_FLOAT)
	{
		*m_pFloat = std::stof(m_Value.c_str());
	}
}
