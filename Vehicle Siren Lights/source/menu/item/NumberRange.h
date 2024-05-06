#pragma once

#include "Item.h"

template<class T>
class NumberRange : public Item {
public:
	NumberRange(T* value, T addBy);

	T* m_Value;
	T m_AddBy;
	T m_Min;
	T m_Max;

	bool m_HoldToChange = false;

	void Init();
	void Update();
	void Draw(float x, float y);
	void DrawNumberDisplay(float x, float y, float width, float height);
	void CheckValueChanged(T prevValue, T newValue);
};