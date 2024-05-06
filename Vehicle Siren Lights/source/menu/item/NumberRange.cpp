#include "NumberRange.h"
#include "../Menu.h"

#include "../TextEditor.h"
#include "../TipMessage.h"

#include "../../input/Input.h"
#include "../../TestHelper.h"

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

	T prevVal = *m_Value;

	if ((*m_Value > m_Min)) {
		if (m_HoldToChange) {
			if (Input::GetKey(VK_LEFT)) {
				(*m_Value) -= m_AddBy;
				TipMessage::ShowTip_NumberRangeSetValue();
			}
		}
		else {
			if (Input::GetKeyDown(VK_LEFT)) {
				(*m_Value) -= m_AddBy;
				TipMessage::ShowTip_NumberRangeSetValue();
			}
		}
	}
	
	if ((*m_Value < m_Max)) {
		if (m_HoldToChange) {
			if (Input::GetKey(VK_RIGHT)) {
				(*m_Value) += m_AddBy;
				TipMessage::ShowTip_NumberRangeSetValue();
			}
		}
		else {
			if (Input::GetKeyDown(VK_RIGHT)) {
				(*m_Value) += m_AddBy;
				TipMessage::ShowTip_NumberRangeSetValue();
			}
		}
	}

	if (Input::GetKeyDown(VK_SPACE)) //SPACE
	{
		if (!TextEditor::m_Visible)
		{
			NumberRange<T>* self = this;

			if (std::is_same<T, float>::value)
			{
				TestHelper::AddLine("start edit float (" + m_Label + ")");
				TextEditor::m_OnConfirm = [self, prevVal]() {
					TestHelper::AddLine("confirm edit float");
					self->CheckValueChanged(prevVal, *self->m_Value);
				};
				TextEditor::Open(m_Label, false, (float*)m_Value);
			}

			if (std::is_same<T, int>::value)
			{
				TestHelper::AddLine("start edit int (" + m_Label + ")");
				TextEditor::m_OnConfirm = [self, prevVal]() {
					TestHelper::AddLine("confirm edit int");
					self->CheckValueChanged(prevVal, *self->m_Value);
				};
				TextEditor::Open(m_Label, false, (int*)m_Value);
			}

			if (std::is_same<T, unsigned char>::value)
			{
				TestHelper::AddLine("start edit edit uchar (" + m_Label + ")");
				TextEditor::m_OnConfirm = [self, prevVal]() {
					TestHelper::AddLine("confirm uchar");
					self->CheckValueChanged(prevVal, *self->m_Value);
				};
				TextEditor::Open(m_Label, false, (unsigned char*)m_Value);
			}
		}
	}

	CheckValueChanged(prevVal, *m_Value);
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

template<class T>
void NumberRange<T>::CheckValueChanged(T prevValue, T newValue)
{
	if (*m_Value < m_Min) *m_Value = m_Min;
	if (*m_Value > m_Max) *m_Value = m_Max;

	if (prevValue != newValue)
	{
		TestHelper::AddLine("value changed from " + std::to_string(prevValue) + " to " + std::to_string(newValue));
		if (m_OnChange)
		{
			m_OnChange();
		}
	}
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