#include "Item.h"
#include "../Menu.h"
#include "../../input/Input.h"

Item::Item() {}

void Item::Draw(float x, float y) {
	CRGBA bgColor = m_IsSelected ? m_BackgroundColorSelected : m_BackgroundColor;
	CRGBA textColor = m_IsSelected ? m_TextColorSelected : m_TextColor;

	Menu::DrawRect(x, y, m_Size.x, m_Size.y, bgColor);
	Menu::DrawString(m_Label, x + m_LabelMargin.x, y + m_LabelMargin.y +  m_Size.y / 2 - 10, textColor);

	Menu::m_FontAlign = eFontAlignment::ALIGN_RIGHT;
	Menu::DrawString(m_RightLabel, x + m_Size.x - 5.0f, y + m_Size.y / 2 - 10, textColor);
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;
	//Menu::DrawString(m_CanBeSelected ? "Y" : "N", x + 30.0f, y + m_Size.y / 2 - 10, textColor);

	if (m_OnDraw) m_OnDraw(x, y);

	if (m_ColorIndicatorValue != nullptr) {
		Menu::DrawRect(x + m_ColorIndicatorX, y + m_Size.y/2 - m_ColorIndicatorSize.y/2, m_ColorIndicatorSize.x, m_ColorIndicatorSize.y, *m_ColorIndicatorValue);
	}

	for (auto a : m_Numbers)
	{
		Menu::m_FontAlign = a.align;
		Menu::DrawString(std::to_string(*a.ptr), x + m_Size.x - 5.0f - a.position.x, y + m_Size.y / 2 - 10 + - a.position.y, textColor);
		Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;
	}

	for (auto a : m_Strings)
	{
		Menu::m_FontAlign = a.align;
		Menu::DrawString(*a.ptr, x + m_Size.x - 5.0f - a.position.x, y + m_Size.y / 2 - 10 + -a.position.y, textColor);
		Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;
	}
}

void Item::Update() {
	if (m_OnUpdate) m_OnUpdate();

	if (Input::GetKeyDown(VK_SPACE) && m_OnClick && m_IsSelected) {
		Menu::m_ItemClicked = this;
	}
}

ItemText<int> Item::AddTextInt(int* value, CVector2D position) {
	ItemText<int> itemText;
	itemText.ptr = value;
	itemText.position = position;
	itemText.align = eFontAlignment::ALIGN_RIGHT;
	m_Numbers.push_back(itemText);
	return itemText;
}

ItemText<std::string> Item::AddTextStr(std::string* value, CVector2D position) {
	ItemText<std::string> itemText;
	itemText.ptr = value;
	itemText.position = position;
	itemText.align = eFontAlignment::ALIGN_RIGHT;
	m_Strings.push_back(itemText);
	return itemText;
}