#pragma once

#include "plugin.h"

#include "item/Item.h"
#include "item/CheckBox.h"
#include "item/Options.h"
#include "item/NumberRange.h"
#include "item/ButtonKey.h"

class Window {
public:
	enum FIND_INDEX_DIR
	{
		UP,
		DOWN
	};

	enum FIND_INDEX_TYPE
	{
		FIRST,
		LAST
	};

	CVector2D m_Position;
	std::vector<Item*> m_Items;
	int m_SelectedIndex = 0;
	int m_MaxItemsByPage = 16;

	std::string m_Title = "";
	std::string m_Description = "";

	CVector2D m_Size = CVector2D(350, 0);

	Window* m_PrevWindow = nullptr;

	//int m_ListStartIndex = 0; //remove
	//int m_ = 0;

	void Draw();
	void GoUp();
	void GoDown();
	void GoBackToPrevWindow();
	void Update();
	void Destroy();
	void CheckSelected(int add);

	//int GetViewTopItemIndex();
	//int GetViewBottomItemIndex();

	Item* GetSelectedItem();
	//int _GetNextSelectableItemIndex(int at, int by);
	int FindSelectableItemIndex(int at, FIND_INDEX_DIR dir, FIND_INDEX_TYPE type);
	int GetBottomMostIndex();
	int GetTopMostIndex();
	Item* AddItem(Item* item);
	Item* AddItem(std::string text);
	Item* AddButton(std::string text);

	CheckBox* AddCheckBox(std::string text, bool* value);
	Options* AddOptions(std::string text, int* value);

	template<class T>
	NumberRange<T>* AddNumberRange(std::string text, T* value, T min, T max);
	Item* AddDivider();
	Item* AddDivider(float height);

	ButtonKey* AddButtonKey(std::string text, Keybind* keybind);
};