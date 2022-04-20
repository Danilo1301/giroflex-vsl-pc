#include "NumberRange.h"
#include "../Menu.h"
#include "../../input/Input.h"

template<class T>
NumberRange<T>::NumberRange(T* value, T addBy)
{
	m_Value = value;
	m_AddBy = addBy;
	m_Min = 0;
	m_Max = 999;
	Init();
}

NumberRange<float>::NumberRange(float* value, float addBy)
{
	m_Value = value;
	m_AddBy = addBy;
	m_Min = -999.0f;
	m_Max = 999.0f;
	Init();
}

NumberRange<int>::NumberRange(int* value, int addBy)
{
	m_Value = value;
	m_AddBy = addBy;
	m_Min = 0;
	m_Max = 999;
	Init();
}

NumberRange<unsigned char>::NumberRange(unsigned char* value, unsigned char addBy)
{
	m_Value = value;
	m_AddBy = addBy;
	m_Min = (unsigned char)0;
	m_Max = (unsigned char)255;
	Init();
}

template<class T>
void NumberRange<T>::Init() {
	m_CanBeSelected = true;

	m_Label = "Number Range";

	m_OnUpdate = std::bind(&NumberRange::Update, this);
	m_OnDraw = std::bind(&NumberRange::Draw, this, std::placeholders::_1, std::placeholders::_2);
}

template<class T>
void NumberRange<T>::Update()
{
	//m_TextSelected->color = CRGBA(0, 0, 0);

	//m_TextValue->str = std::to_string(*m_Value);

	if (!m_IsSelected) return;

	if (m_HoldToChange) {
		if (Input::GetKey(VK_LEFT)) {
			(*m_Value) -= m_AddBy;
		}
	}
	else {
		if (Input::GetKeyDown(VK_LEFT)) {
			(*m_Value) -= m_AddBy;
		}
	}

	if (m_HoldToChange) {
		if (Input::GetKey(VK_RIGHT)) {
			(*m_Value) += m_AddBy;
		}
	}
	else {
		if (Input::GetKeyDown(VK_RIGHT)) {
			(*m_Value) += m_AddBy;
		}
	}

	if (*m_Value < m_Min) *m_Value = m_Min;
	if (*m_Value > m_Max) *m_Value = m_Max;
}

template<class T>
void NumberRange<T>::Draw(float x, float y)
{
	float displayW = 150.0f;
	float displayH = 20.0f;
	DrawNumberDisplay(x + m_Size.x - (displayW) - 10, y + m_Size.y/2 - (displayH)/2, displayW, displayH);
}

template<class T>
void NumberRange<T>::DrawNumberDisplay(float x, float y, float width, float height)
{
	Menu::DrawRect(x, y, width, height, CRGBA(255, 255, 255, 120));
	Menu::DrawArrow(x + 5, y + 5, eArrowDirection::LEFT, height - 10, height - 10, CRGBA(0, 0, 0));
	Menu::DrawArrow(x + 5 + width - height, y + 5, eArrowDirection::RIGHT, height - 10, height - 10, CRGBA(0, 0, 0));

	Menu::m_FontAlign = eFontAlignment::ALIGN_CENTER;
	Menu::DrawString(std::to_string(*m_Value), x + width/2, y + height/2 - 10.0f, CRGBA(0, 0, 0));
	Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;
}

/*

void NumberRange::Update() {

	m_TextSelected->color = CRGBA(0, 0, 0);

	if (!m_IsSelected) return;

	if (Input::GetKey(VK_LEFT)) {

		m_TextSelected->color = CRGBA(0, 255, 0);
	}
	if (Input::GetKey(VK_RIGHT)) {

		m_TextSelected->color = CRGBA(255, 0, 0);
	}

}
*/