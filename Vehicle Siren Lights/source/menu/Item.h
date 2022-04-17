#pragma once

#include "plugin.h"
#include "CFont.h"

/*
struct Text {
	std::string str;
	CVector2D position;
	CRGBA color;
	eFontAlignment align;
};

struct Rect {
	CVector2D position;
	CVector2D size;
	CRGBA color;
};
*/

class Item {
public:
	//std::vector<Text*> m_Texts;
	//std::vector<Rect*> m_Rects;

	CVector2D m_Size = CVector2D(300, 30);

	std::string m_Label = "Item";
	CVector2D m_LabelMargin = CVector2D(5, 0);

	std::string m_RightLabel = "";
	CRGBA m_BackgroundColor = CRGBA(0, 0, 0, 0);
	CRGBA m_BackgroundColorSelected = CRGBA(255, 255, 255, 125);

	CRGBA m_TextColor = CRGBA(255, 255, 255);
	CRGBA m_TextColorSelected = CRGBA(0, 0, 0);

	float m_ColorIndicatorX = 10.0f;
	CVector2D m_ColorIndicatorSize = CVector2D(20, 20);
	CRGBA* m_ColorIndicatorValue = nullptr;


	bool m_CanBeSelected = false;
	bool m_IsSelected = false;

	std::function<void(void)> m_OnUpdate; //-
	std::function<void(float, float)> m_OnDraw; //-

	//Text* m_TextLabel;
	//Text* m_TextSelected;


	Item();

	void Draw(float x, float y);
	void Update();

	//Text* AddText(std::string text, float x, float y);
	//Rect* AddRect(float x, float y, float width, float height, CRGBA color);
};