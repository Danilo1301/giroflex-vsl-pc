#include "KeySelector.h"

#include "../TestHelper.h"
#include "../menu/Menu.h"

Keybind KeySelector::m_KeybindResult("", 0);
Keybind* KeySelector::m_KeybindTarget = NULL;
bool KeySelector::m_Visible = false;
std::function<void(void)> KeySelector::m_OnConfirm = NULL;

void KeySelector::Toggle(Keybind* keybind)
{
	if (keybind) {
		m_KeybindTarget = keybind;
		TestHelper::AddLine("[KeySelector] Set keybind");
	}

	if (m_Visible)
	{
		TestHelper::AddLine("[KeySelector] Apply keybind");

		m_KeybindTarget->KeyCode = m_KeybindResult.KeyCode;
		m_KeybindTarget->Flags = m_KeybindResult.Flags;
		m_KeybindTarget = NULL;

		if (m_OnConfirm) m_OnConfirm();
		m_OnConfirm = NULL;
	}

	m_Visible = !m_Visible;
}

void KeySelector::Update()
{
	if (!m_Visible) return;

	bool ctrl = Input::GetKey(17);
	bool alt = Input::GetKey(18);
	bool shift = Input::GetKey(16);

	for (int i = 0; i < 256; i++)
	{
		if (i == 13 || i == 32 || i == 16 || i == 17 || i == 18 || i == 160 || i == 161 || i == 162 || i == 163 || i == 164) continue;

		if (Input::GetKey(i))
		{
			TestHelper::AddLine("[KeySelector] " + std::to_string(i));

			m_KeybindResult.Flags = 0;
			if (ctrl) m_KeybindResult.Flags |= KEYBIND_FLAGS::CTRL;
			if (alt) m_KeybindResult.Flags |= KEYBIND_FLAGS::ALT;
			if (shift) m_KeybindResult.Flags |= KEYBIND_FLAGS::SHIFT;

			m_KeybindResult.KeyCode = i;
		}
	}

	if (Input::GetKeyDown(13)) {
		Toggle(NULL);
	}
}

void KeySelector::Draw()
{
	float width = 200.0f;
	float height = 70.0f;

	float x = GetFullScreenSizeX() / 2 - width / 2;
	float y = GetFullScreenSizeY() - 10.0f - height;

	Menu::DrawRect(x, y, width, height, CRGBA(0, 0, 0, 190));

	Menu::m_FontAlign = eFontAlignment::ALIGN_CENTER;
	Menu::DrawString("[Press any key combination]", x + width / 2, y + 10.0f, CRGBA(255, 255, 255));
	Menu::DrawString(m_KeybindResult.GetKeybindString(), x + width / 2, y + 30.0f, CRGBA(255, 255, 255));
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;
}