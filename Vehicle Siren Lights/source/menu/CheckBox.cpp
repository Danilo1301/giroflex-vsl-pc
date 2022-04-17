#include "CheckBox.h"
#include "Menu.h"
#include "../input/Input.h"

CheckBox::CheckBox(bool* value) {
	m_Value = value;
	m_CanBeSelected = true;

	m_OnUpdate = std::bind(&CheckBox::Update, this);
	m_OnDraw = std::bind(&CheckBox::Draw, this, std::placeholders::_1, std::placeholders::_2);
}

void CheckBox::Update() {
	if (!m_IsSelected) return;

	if (Input::GetKeyDown(VK_SPACE)) {
		(*m_Value) = !(*m_Value);
	}
}

void CheckBox::Draw(float x, float y) {

	CVector2D checkboxSize = CVector2D(m_Size.y - 10, m_Size.y - 10);
	CVector2D checkboxPos = CVector2D(x + m_Size.x - 10 - checkboxSize.x, y + m_Size.y / 2 - checkboxSize.y / 2);

	Menu::DrawRect(
		checkboxPos.x,
		checkboxPos.y,
		checkboxSize.x,
		checkboxSize.y,
		CRGBA(0, 0, 0)
	);

	Menu::DrawRect(
		checkboxPos.x + 2,
		checkboxPos.y + 2,
		checkboxSize.x - 4,
		checkboxSize.y - 4,
		(*m_Value) ? CRGBA(255, 255, 255) : CRGBA(70, 70, 70)
	);
}