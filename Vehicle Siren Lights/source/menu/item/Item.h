#pragma once

#include "plugin.h"
#include "CFont.h"

template <class T>
struct ItemText {
	T* ptr;
	CVector2D position;
	eFontAlignment align;
};

class Item {
public:
	CVector2D m_Size = CVector2D(300, 30);

	std::string m_Label = "Item";
	CVector2D m_LabelMargin = CVector2D(5, 0);

	std::string m_RightLabel = "";

	CRGBA m_BackgroundColor = CRGBA(0, 0, 0, 0);
	CRGBA m_BackgroundColorSelected = CRGBA(255, 255, 255, 125);

	CRGBA m_TextColor = CRGBA(255, 255, 255);
	CRGBA m_TextColorSelected = CRGBA(0, 0, 0);

	std::vector<ItemText<std::string>> m_Strings;
	std::vector<ItemText<int>> m_Numbers;
	std::vector<ItemText<CRGBA>> m_ColorIndicators;

	bool m_CanBeSelected = false;
	bool m_IsSelected = false;

	std::function<void(void)> m_OnUpdate;
	std::function<void(float, float)> m_OnDraw;
	std::function<void(void)> m_OnClick;
	std::function<void(void)> m_OnChange;

	Item();

	void Draw(float x, float y);
	void Update();

	ItemText<int> AddTextInt(int* value, CVector2D position);
	ItemText<std::string> AddTextStr(std::string* value, CVector2D position);
	ItemText<CRGBA> AddColorIndicator(CRGBA* value, CVector2D position);
};