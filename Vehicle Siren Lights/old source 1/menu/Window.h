#pragma once

#include "plugin.h"

#include "Item.h"
#include "CheckBox.h"
#include "Button.h"
#include "Options.h"
#include "NumberRange.h"
#include "ButtonKey.h"

class Window {
public:
	CVector2D m_Position;
	std::vector<Item*> m_Items;
	int m_SelectedIndex = 0;

	std::string m_Title = "Title";
	std::string m_Description = "Desc";

	CVector2D m_Size = CVector2D(400, 0);

	int m_ListStartIndex = 0; //remove
	int m_ListIndex = 0;
	int m_MaxItems = 15;

	void Draw();
	void GoUp();
	void GoDown();
	void Update();
	void Destroy();
	void CheckSelected(int add);

	int GetViewTopItemIndex();
	int GetViewBottomItemIndex();

	Item* GetSelectedItem();
	int GetNextSelectableItemIndex(int by);
	Item* AddItem(Item* item);

	CheckBox* AddCheckBox(std::string text, bool* value);
	Button* AddButton(std::string text);
	Options* AddOptions(std::string text, int* value);

	template<class T>
	NumberRange<T>* AddNumberRange(std::string text, T* value, T min, T max);
	void AddDivider(float height);

	ButtonKey* AddButtonKey(std::string text, int* keys);

};