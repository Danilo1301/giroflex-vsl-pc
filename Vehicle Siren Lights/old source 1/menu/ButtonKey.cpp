#include "ButtonKey.h"
#include "Menu.h"
#include "../input/Input.h"
#include "TextEditor.h"

ButtonKey* ButtonKey::m_EditingButtonKey = NULL;

std::string mapKey(int key) {
	char c = MapVirtualKey(key, MAPVK_VK_TO_CHAR);
	auto lower = std::string(1, std::tolower(c));
	auto upper = std::string(1, std::toupper(c));

	if (lower.empty()) return "EMPTY";

	switch (key)
	{
	case 16:
		return "SHIFT";
	case 17:
		return "CTRL";
	case 18:
		return "ALT";
	}

	if (TextEditor::m_AvailableChars.find(lower) != std::string::npos) {
		return upper;
	}

	return std::to_string(key);
}

ButtonKey::ButtonKey(int* value) {
	m_CanBeSelected = true;
	m_Keys = value;

	m_OnUpdate = std::bind(&ButtonKey::Update, this);
	m_OnDraw = std::bind(&ButtonKey::Draw, this, std::placeholders::_1, std::placeholders::_2);
}

void ButtonKey::Update() {
	if (m_EditingButtonKey) {
		int keysBeingPressed = 0;
		int totalKeys = 0;
		for (int i = 0; i <= 2; i++)
		{
			if (m_Keys[i] == -1) continue;
			totalKeys++;
			if (Input::GetKey(m_Keys[i])) keysBeingPressed++;
		}

		if (totalKeys < 3) {
			for (int i = 0; i < 200; i++) {
				if (i == 32) continue;
				if (i == 162) continue;
				if (i == 164) continue;

				if (Input::GetKey(i)) {

					bool alreadyAdded = false;
					for (int i2 = 0; i2 < 3; i2++)
					{
						if (m_Keys[i2] == i) {
							alreadyAdded = true;
							break;
						}
					}

					if (!alreadyAdded) {
						m_Keys[totalKeys] = i;
						break;
					}
					
				}
			}
		}
		
		if (totalKeys > 0 && totalKeys > keysBeingPressed) {
			m_EditingButtonKey = NULL;
		}

		/*
		for (int i = 0; i < 200; i++) {
			if (i == 32) continue;

			if (Input::GetKey(i)) {
				if (i >= 16 && i <= 18) {
					m_Keys[m_Keys[0] == -1 ? 0 : 1] = i;
				}
				else {
					m_Keys[2] = i;
				}
			}
		}

		
		int keysBeingPressed = 0;
		int totalKeys = 0;
		for (int i = 0; i <= 2; i++)
		{
			if (m_Keys[i] == -1) continue;
			totalKeys++;
			if (Input::GetKey(m_Keys[i])) keysBeingPressed++;
		}

		if (totalKeys > 0 && totalKeys > keysBeingPressed) {
			m_EditingButtonKey = NULL;
		}
		*/
	}

	if (!m_IsSelected) return;

	if (Input::GetKeyDown(VK_SPACE)) {
		if (!m_EditingButtonKey) {
			m_Keys[0] = -1;
			m_Keys[1] = -1;
			m_Keys[2] = -1;
			m_EditingButtonKey = this;
		}
		else {
			m_EditingButtonKey = NULL;
		}
	}
}

void ButtonKey::Draw(float x, float y) {
	float displayW = 150.0f;
	float displayH = 20.0f;
	DrawTextDisplay(x + m_Size.x - (displayW)-10, y + m_Size.y / 2 - (displayH) / 2, displayW, displayH);
}

void ButtonKey::DrawTextDisplay(float x, float y, float width, float height)
{
	Menu::DrawRect(x, y, width, height, CRGBA(255, 255, 255));

	std::string text = "( ";
	if (m_Keys[0] != -1) text += mapKey(m_Keys[0]);
	if (m_Keys[1] != -1) text += " + " + mapKey(m_Keys[1]);
	if (m_Keys[2] != -1) text += " + " + mapKey(m_Keys[2]);

	if (m_EditingButtonKey) {
		text += " [editing]";
	}
	text += " )";

	Menu::m_FontAlign = eFontAlignment::ALIGN_CENTER;
	Menu::DrawString(text, x + width / 2, y + height / 2 - 10.0f, CRGBA(0, 0, 0));
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;

}