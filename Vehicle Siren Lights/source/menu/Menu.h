#pragma once

#include "../pch.h"
#include "Window.h"

enum class eArrowDirection {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Menu {
public:
	static std::string m_DefaultWindowTitle;
	static std::vector<Window*> m_Windows;
	static Window* m_ActiveWindow;
	static Item* m_ItemClicked;

	static bool m_IsOpen;
	static bool m_Hide;
	static int m_OpenAtIndex;
	static CVector2D m_DefaultPosition;

	static eFontAlignment m_FontAlign;
	static eFontStyle m_FontStyle;

	static void DrawRect(float x, float y, float width, float height, CRGBA color);
	static void DrawString(std::string text, float x, float y, CRGBA color);
	static void DrawArrow(float x, float y, eArrowDirection direction, float width, float height, CRGBA color);
	static void DrawArrowButton(float x, float y, eArrowDirection direction, float width, float height, bool active);
	static void DrawKeyButton(std::string text, float x, float y, float width, float height);
	static void DrawArrowKeyButton(float x, float y, eArrowDirection direction, float width, float height);

	static void DrawListArrowIndicator(float x, float y, eArrowDirection direction, float width, float height);

	static Window* AddWindow(std::string title, std::string description);
	static Window* AddWindow(std::string title);
	static Window* AddWindow();
	static void RemoveWindow(Window* window);
	static void RemoveAllWindows();
	static Window* CreateColorPickerWindow(CRGBA* color, std::function<void(void)> onClose);
	static Window* CreateConfirmWindow(std::string title, std::string message, std::string yes, std::string no, std::function<void(void)> onConfirm);

	static void SetOpen(bool open);
	static bool Toggle();
	static void Draw();
	static void Update();
	static void SaveIndex();
	static void RestoreIndex();
};