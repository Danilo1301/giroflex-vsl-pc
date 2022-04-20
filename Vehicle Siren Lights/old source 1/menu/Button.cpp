#include "Button.h"
#include "Menu.h"
#include "../input/Input.h"

Button::Button() {
	m_CanBeSelected = true;

	m_OnUpdate = std::bind(&Button::Update, this);
	m_OnDraw = std::bind(&Button::Draw, this, std::placeholders::_1, std::placeholders::_2);
}

void Button::Update() {
	if (!m_IsSelected) return;

	if (Input::GetKeyDown(VK_SPACE)) {
		OnButtonClicked();
	}

}

void Button::Draw(float x, float y) {
}

void Button::OnButtonClicked() {
	Menu::m_ButtonClicked = this;
}