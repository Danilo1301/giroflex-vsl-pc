#include "ButtonKey.h"
#include "../Menu.h"
#include "../../input/Input.h"
#include "../TextEditor.h"

ButtonKey::ButtonKey(Keybind* keybind) {
	m_CanBeSelected = true;
	m_Keybind = keybind;

	m_OnUpdate = std::bind(&ButtonKey::Update, this);
	m_OnDraw = std::bind(&ButtonKey::Draw, this, std::placeholders::_1, std::placeholders::_2);
}

void ButtonKey::Update() {
	
}

void ButtonKey::Draw(float x, float y) {
	float displayW = 150.0f;
	float displayH = 20.0f;
	DrawTextDisplay(x + m_Size.x - (displayW)-10, y + m_Size.y / 2 - (displayH) / 2, displayW, displayH);
}

void ButtonKey::DrawTextDisplay(float x, float y, float width, float height)
{
	Menu::DrawRect(x, y, width, height, CRGBA(255, 255, 255));

	Menu::m_FontAlign = eFontAlignment::ALIGN_CENTER;
	Menu::DrawString(m_Keybind->GetKeybindString(), x + width / 2, y + height / 2 - 10.0f, CRGBA(0, 0, 0));
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;

}