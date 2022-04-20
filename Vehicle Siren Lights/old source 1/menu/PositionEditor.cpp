#include "PositionEditor.h"
#include "../input/Input.h"

bool PositionEditor::m_Visible = false;
CVector* PositionEditor::m_Value = nullptr;

void PositionEditor::Update() {
	CVector* value = m_Value;

	if (value == nullptr) return;

	if (Input::GetKey(VK_LEFT)) {
		(*value).x -= 0.01f;
	}
	if (Input::GetKey(VK_RIGHT)) {
		(*value).x += 0.01f;
	}

	if (Input::GetKey(VK_UP)) {
		if (Input::GetKey(VK_LCONTROL)) {
			(*value).z += 0.01f;
		}
		else {
			(*value).y += 0.01f;
		}
	}

	if (Input::GetKey(VK_DOWN)) {
		if (Input::GetKey(VK_LCONTROL)) {
			(*value).z -= 0.01f;
		}
		else {
			(*value).y -= 0.01f;
		}
	}

	if (Input::GetKeyDown(13)) {
		Toggle(nullptr);
	}
}

void PositionEditor::Draw() {
	CVector2D size = CVector2D(500, 120);
	CVector2D position = CVector2D(
		1024.0f / 2 - size.x / 2,
		768.0f - size.y - 80.0f
	);
	CRGBA textColor = CRGBA(255, 255, 255);

	Menu::DrawRect(position.x, position.y, size.x, size.y, CRGBA(0, 0, 0, 190));

	Menu::DrawString("Title here", position.x + 10.0f, position.y + 10.0f, textColor);

	CVector2D keys1Position = CVector2D(position.x + 100.0f, position.y);
	Menu::DrawArrowKeyButton(keys1Position.x + (20 + 2) * 0, keys1Position.y, eArrowDirection::LEFT, 20, 20);
	Menu::DrawArrowKeyButton(keys1Position.x + (20 + 2) * 1, keys1Position.y, eArrowDirection::DOWN, 20, 20);
	Menu::DrawArrowKeyButton(keys1Position.x + (20 + 2) * 1, keys1Position.y - (20 + 2), eArrowDirection::UP, 20, 20);
	Menu::DrawArrowKeyButton(keys1Position.x + (20 + 2) * 2, keys1Position.y, eArrowDirection::RIGHT, 20, 20);
	Menu::DrawString("Move", keys1Position.x + 70, keys1Position.y, CRGBA(255, 255, 255));

	CVector2D keys2Position = CVector2D(position.x + 200.0f, position.y);
	Menu::DrawKeyButton("CTRL", keys2Position.x, keys2Position.y, 50, 20);
	Menu::DrawString("+", keys2Position.x + 55.0f, keys2Position.y, CRGBA(255, 255, 255));
	Menu::DrawArrowKeyButton(70 + keys2Position.x, keys2Position.y - 15, eArrowDirection::UP, 20, 20);
	Menu::DrawArrowKeyButton(70 + keys2Position.x, keys2Position.y + 15, eArrowDirection::DOWN, 20, 20);
	Menu::DrawString("Move Up/Down", keys2Position.x + 100, keys2Position.y, CRGBA(255, 255, 255));

	CVector2D key3Position = CVector2D(position.x + 100.0f, position.y + 100.0f);
	Menu::DrawKeyButton("ENTER", key3Position.x, key3Position.y, 50, 20);
	Menu::DrawString("Confirm", key3Position.x + 60.0f, key3Position.y, CRGBA(255, 255, 255));

	if (m_Value == nullptr) return;

	CVector* value = m_Value;

	Menu::DrawString("X: " + std::to_string(value->x), position.x + 10.0f, position.y + 40, textColor);
	Menu::DrawString("Y: " + std::to_string(value->y), position.x + 10.0f, position.y + 60, textColor);
	Menu::DrawString("Z: " + std::to_string(value->z), position.x + 10.0f, position.y + 80, textColor);

	/*
	CVector2D position = CVector2D(400, 400);
	CVector2D size = CVector2D(200, 200);

	Menu::DrawRect(position.x, position.y, size.x, size.y, CRGBA(255, 255, 255, 100));

	if (m_Value == nullptr) return;

	CVector* value = m_Value;

	Menu::DrawString(std::to_string(value->x), position.x, position.y, CRGBA(0, 0, 0));
	Menu::DrawString(std::to_string(value->y), position.x, position.y + 20, CRGBA(0, 0, 0));
	Menu::DrawString(std::to_string(value->z), position.x, position.y + 40, CRGBA(0, 0, 0));
	*/
}

void PositionEditor::Toggle(CVector* value) {
	m_Visible = !m_Visible;

	m_Value = nullptr;
	if (m_Visible) m_Value = value;
}