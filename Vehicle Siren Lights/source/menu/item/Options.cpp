#include "Options.h"
#include "../Menu.h"
#include "../../input/Input.h"

Options::Options(int* value) {
	m_Value = value;
	m_CanBeSelected = true;

	m_Label = "Options";

	m_OnUpdate = std::bind(&Options::Update, this);
	m_OnDraw = std::bind(&Options::Draw, this, std::placeholders::_1, std::placeholders::_2);
}

void Options::Update() {
	if (!m_IsSelected) return;

	if (Input::GetKeyDown(VK_LEFT)) {
		ChangeOption(-1);
	}

	if (Input::GetKeyDown(VK_RIGHT)) {
		ChangeOption(1);
	}
}

void Options::Draw(float x, float y) {
	float displayW = 150.0f;
	float displayH = 20.0f;
	DrawNumberDisplay(x + m_Size.x - (displayW)-10, y + m_Size.y / 2 - (displayH) / 2, displayW, displayH);
}

void Options::DrawNumberDisplay(float x, float y, float width, float height)
{
	std::string text = "?";

	int optionIndex = GetOptionIndexByValue(*m_Value);
	if(optionIndex != -1) text = m_Options[optionIndex].text;

	float margin = 2.0f;

	Menu::DrawRect(x - margin, y - margin, width + margin * 2, height + margin * 2, CRGBA(255, 255, 255, 180));

	Menu::m_FontAlign = eFontAlignment::ALIGN_CENTER;
	Menu::DrawString(text, x + width / 2, y + height / 2 - 10.0f, CRGBA(0, 0, 0));
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;

	Menu::DrawArrowButton(x, y, eArrowDirection::LEFT, height, height, optionIndex != 0);
	Menu::DrawArrowButton(x + width - height, y, eArrowDirection::RIGHT, height, height, optionIndex < (int)m_Options.size() - 1);
}

void Options::AddOption(std::string text, int value) {
	Option option = { text, value };
	m_Options.push_back(option);
}

int Options::GetOptionIndexByValue(int value) {
	int i = 0;
	for (Option option : m_Options) {
		if (option.value == value) return i;
		i++;
	}
	return -1;
}

void Options::ChangeOption(int by) {
	int next = GetOptionIndexByValue(*m_Value) + by;

	if (next >= 0 && next < (int)m_Options.size()) {
		*m_Value = m_Options[next].value;
	}

	if (m_OnChange) m_OnChange();
}