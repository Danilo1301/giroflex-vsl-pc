#include "Window.h"
#include "Menu.h"

void Window::Draw() {
	char buffer[256];
	CVector2D drawPos = CVector2D(m_Position.x, m_Position.y);

	//
	if (m_Title.length() > 0) {
		Menu::DrawRect(drawPos.x, drawPos.y, m_Size.x, 50, CRGBA(56, 172, 255));
		Menu::DrawString(m_Title, drawPos.x + 10, drawPos.y + 50 / 2 - 10, CRGBA(255, 255, 255));
		drawPos.y += 50.0f;
	}

	if (m_Description.length() > 0) {
		Menu::DrawRect(drawPos.x, drawPos.y, m_Size.x, 20, CRGBA(0, 0, 0, 200));
		Menu::DrawString(m_Description, drawPos.x + 5, drawPos.y + 20 / 2 - 10, CRGBA(255, 255, 255));
		drawPos.y += 20.0f;
	}

	//

	

	//

	std::vector<Item*> items;

	float totalSize = 0.0f;
	int i = 0;
	int startAt = m_ListStartIndex + m_ListIndex;
	for (Item* item : m_Items) {
		if (i >= startAt && i < startAt + m_MaxItems) {
			items.push_back(item);
			totalSize += item->m_Size.y;
		}
		i++;
	}
	m_Size.y = totalSize;

	//

	bool canDrawListArrows = items.size() != m_Items.size();

	if (canDrawListArrows) {
		Menu::DrawListArrowIndicator(drawPos.x, drawPos.y, eArrowDirection::UP, m_Size.x, 15.0f);
		drawPos.y += 15.0f;
	}


	Menu::DrawRect(drawPos.x, drawPos.y, m_Size.x, m_Size.y, CRGBA(0, 0, 0, 190));
	
	for (Item* item : items) {
		//item->m_Size.x = m_Size.x;
		item->Draw(drawPos.x, drawPos.y);
		drawPos.y += item->m_Size.y;
	}

	//
	if (canDrawListArrows) {
		Menu::DrawListArrowIndicator(drawPos.x, drawPos.y, eArrowDirection::DOWN, m_Size.x, 15.0f);
		drawPos.y += 15.0f;
	}
	//

	sprintf_s(buffer, "%d next 1: %d , -1: %d -- top %d bottom %d", m_SelectedIndex, GetNextSelectableItemIndex(1), GetNextSelectableItemIndex(-1), GetViewTopItemIndex(), GetViewBottomItemIndex());
	Menu::DrawString(buffer, m_Position.x, m_Position.y, CRGBA(255,0,0));
}


void Window::GoUp() {
	int newIndex = GetNextSelectableItemIndex(-1);
	int diff = newIndex - m_SelectedIndex;
	m_SelectedIndex = newIndex;

	if (m_SelectedIndex <= GetViewTopItemIndex() - 1) {
		m_ListIndex += diff;
	}
}

void Window::GoDown() {
	int newIndex = GetNextSelectableItemIndex(1);
	int diff = newIndex - m_SelectedIndex;
	m_SelectedIndex = newIndex;

	if (m_SelectedIndex >= GetViewBottomItemIndex() + 1) {
		m_ListIndex += diff;
	}
	//CheckSelected(1);
}

void Window::Update() {
	for (Item* item : m_Items) {
		item->m_IsSelected = false;

		if (item->m_CanBeSelected && Menu::m_ActiveWindow == this) 
			item->m_IsSelected = GetSelectedItem() == item;
		
		item->Update();
	}

	if (m_SelectedIndex == 0 && m_Items.size() > 0) {
		if (!m_Items[0]->m_CanBeSelected) {
			GoDown();
		}
	}
	//CheckSelected(1);
}

void Window::Destroy() {
	for (Item* item : m_Items) {
		delete item;
	}
	m_Items.clear();
}

void Window::CheckSelected(int add) {
	if (m_SelectedIndex < 0) return;

	Item* selectedItem = GetSelectedItem();
	if (selectedItem) {
		if (!selectedItem->m_CanBeSelected) {
			m_SelectedIndex += add;
			CheckSelected(add);
		}
	}
}

int Window::GetViewTopItemIndex() {
	return m_ListStartIndex + m_ListIndex;
}

int Window::GetViewBottomItemIndex() {
	return m_ListStartIndex + m_ListIndex + m_MaxItems - 1;
}

Item* Window::GetSelectedItem() {
	if (m_SelectedIndex < 0 || m_SelectedIndex >= (int)m_Items.size()) return nullptr;
	return m_Items[m_SelectedIndex];
}

int Window::GetNextSelectableItemIndex(int by) {
	//if (m_SelectedIndex < 0 || m_SelectedIndex >= (int)m_Items.size()) return nullptr;

	int find = m_SelectedIndex + by;

	while (find >= 0 && find < (int)m_Items.size())
	{
		if (m_Items[find]->m_CanBeSelected) {
			return find;
		}
		find += by;
	}

	return m_SelectedIndex;
}

Item* Window::AddItem(Item* item) {
	item->m_Size.x = m_Size.x;
	m_Items.push_back(item);
	return item;
}

CheckBox* Window::AddCheckBox(std::string text, bool* value) {
	auto checkBox = (CheckBox*)AddItem((Item*)new CheckBox(value));
	checkBox->m_Label = text;
	return checkBox;
}

Button* Window::AddButton(std::string text) {
	auto button = (Button*)AddItem((Item*)new Button());
	button->m_Label = text;
	return button;
}

Options* Window::AddOptions(std::string text, int* value) {
	auto options = (Options*)AddItem((Item*)new Options(value));
	options->m_Label = text;
	return options;
}

template<class T>
NumberRange<T>* Window::AddNumberRange(std::string text, T* value, T min, T max) {
	auto numberRange = (NumberRange<T>*)AddItem((Item*)new NumberRange(value, 1));
	numberRange->m_Label = text;
	numberRange->m_Min = min;
	numberRange->m_Max = max;
	return numberRange;
}

template<>
NumberRange<int>* Window::AddNumberRange(std::string text, int* value, int min, int max) {
	auto numberRange = (NumberRange<int>*)AddItem((Item*)new NumberRange(value, 1));
	numberRange->m_Label = text;
	numberRange->m_HoldToChange = false;
	numberRange->m_Min = min;
	numberRange->m_Max = max;
	return numberRange;
}

template<>
NumberRange<float>* Window::AddNumberRange(std::string text, float* value, float min, float max) {
	auto numberRange = (NumberRange<float>*)AddItem((Item*)new NumberRange(value, 0.005f));
	numberRange->m_Label = text;
	numberRange->m_HoldToChange = true;
	numberRange->m_Min = min;
	numberRange->m_Max = max;
	return numberRange;
}

template<>
NumberRange<unsigned char>* Window::AddNumberRange(std::string text, unsigned char* value, unsigned char min, unsigned char max) {
	auto numberRange = (NumberRange<unsigned char>*)AddItem((Item*)new NumberRange(value, (unsigned char)1));
	numberRange->m_Label = text;
	numberRange->m_HoldToChange = true;
	numberRange->m_Min = min;
	numberRange->m_Max = max;
	return numberRange;
}

void Window::AddDivider(float height) {
	auto divider = AddItem(new Item());
	divider->m_Size.y = height;
	divider->m_Label = "";
	divider->m_BackgroundColor = CRGBA(255, 255, 255, 255);
}

ButtonKey* Window::AddButtonKey(std::string text, int* keys) {
	auto buttonKey = (ButtonKey*)AddItem(new ButtonKey(keys));
	buttonKey->m_Label = text;
	return buttonKey;
}


/*
NumberRange<float>* Window::AddNumberRange(std::string text, float* value, float min, float max) {
	auto numberRange = (NumberRange<float>)AddItem((Item*)new NumberRange(value, 1.0f));
	numberRange->m_Label = text;
		return numberRange;
}
*/