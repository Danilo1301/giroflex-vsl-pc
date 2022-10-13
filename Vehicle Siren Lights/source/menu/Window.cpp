#include "Window.h"
#include "Menu.h"


void Window::Draw() {
	char buffer[256];
	CVector2D drawPos = CVector2D(m_Position.x, m_Position.y);

	//
	if (m_Title.length() > 0) {
		Menu::DrawRect(drawPos.x, drawPos.y, m_Size.x, 50, CRGBA(56, 172, 255));
		Menu::DrawString(m_Title, drawPos.x + 10, drawPos.y + 50.0f / 2 - 10, CRGBA(255, 255, 255));
		drawPos.y += 50.0f;
	}

	if (m_Description.length() > 0) {
		Menu::DrawRect(drawPos.x, drawPos.y, m_Size.x, 20, CRGBA(0, 0, 0, 200));
		Menu::DrawString(m_Description, drawPos.x + 5, drawPos.y + 20.0f / 2 - 10, CRGBA(255, 255, 255));
		drawPos.y += 20.0f;
	}

	//

	

	//

	std::vector<Item*> items;

	/*
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
	*/

	int page = (int)floor((float)m_SelectedIndex / (float)m_MaxItemsByPage);
	int maxPages = (int)ceil((float)m_Items.size() / (float)m_MaxItemsByPage);
	int startIndex = (page * m_MaxItemsByPage);
	int endIndex = startIndex + m_MaxItemsByPage;
	float totalHeight = 0.0f;

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if ((int)i >= startIndex && (int)i < (startIndex + m_MaxItemsByPage))
		{
			auto item = m_Items[i];

			items.push_back(item);
			totalHeight += item->m_Size.y;
		}
	}

	m_Size.y = totalHeight;

	//

	bool canDrawListArrows = maxPages > 1;

	if (canDrawListArrows)
	{
		if (page > 0)
		{
			Menu::DrawListArrowIndicator(drawPos.x, drawPos.y, eArrowDirection::UP, m_Size.x, 15.0f);
			drawPos.y += 15.0f;
		}
	}

	Menu::DrawRect(drawPos.x, drawPos.y, m_Size.x, m_Size.y, CRGBA(0, 0, 0, 190));

	for (Item* item : items) {
		//item->m_Size.x = m_Size.x;
		item->Draw(drawPos.x, drawPos.y);
		drawPos.y += item->m_Size.y;
	}

	//
	if (canDrawListArrows)
	{
		if (page < maxPages - 1)
		{
			Menu::DrawListArrowIndicator(drawPos.x, drawPos.y, eArrowDirection::DOWN, m_Size.x, 15.0f);
			drawPos.y += 15.0f;
		}
	}
	//

	/*
	sprintf_s(buffer, "selectedIndex=%d maxItems=%d | nextBy(1)=%d nextBy(-1)=%d | page=%d / %d startIndex=%d", //top=%d bottom=%d
		m_SelectedIndex,
		m_MaxItemsByPage,
		FindSelectableItemIndex(m_SelectedIndex, FIND_INDEX_DIR::DOWN, FIND_INDEX_TYPE::FIRST),
		FindSelectableItemIndex(m_SelectedIndex, FIND_INDEX_DIR::UP, FIND_INDEX_TYPE::FIRST),
		page,
		maxPages,
		startIndex
		//GetViewTopItemIndex(),
		//GetViewBottomItemIndex()
	);
	Menu::DrawString(buffer, m_Position.x, m_Position.y, CRGBA(255, 0, 0));

	sprintf_s(buffer, "bottomMost=%d topMost=%d", GetBottomMostIndex(), GetTopMostIndex());
	Menu::DrawString(buffer, m_Position.x, m_Position.y + 20, CRGBA(255, 0, 0));
	*/
}

void Window::GoUp() {
	if (m_SelectedIndex == GetTopMostIndex())
	{
		m_SelectedIndex = GetBottomMostIndex();
		return;
	}

	int newIndex = FindSelectableItemIndex(m_SelectedIndex, FIND_INDEX_DIR::UP, FIND_INDEX_TYPE::FIRST);
	m_SelectedIndex = newIndex;
}

void Window::GoDown() {
	if (m_SelectedIndex == GetBottomMostIndex())
	{
		m_SelectedIndex = 0;
		return;
	}

	int newIndex = FindSelectableItemIndex(m_SelectedIndex, FIND_INDEX_DIR::DOWN, FIND_INDEX_TYPE::FIRST);
	m_SelectedIndex = newIndex;
}

void Window::GoBackToPrevWindow()
{
	if (m_PrevWindow == nullptr) return;

	Menu::m_ActiveWindow = m_PrevWindow;
	Menu::RemoveWindow(this);
}

void Window::Update() {
	for (Item* item : m_Items) {
		//item->m_IsSelected = false;

		if (Menu::m_ActiveWindow == this)
		{
			if (item->m_CanBeSelected)
			{
				item->m_IsSelected = GetSelectedItem() == item;
			}

			item->Update();
		}
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

/*
int Window::GetViewTopItemIndex() {
	return m_ListIndex;
}
*/

/*
int Window::GetViewBottomItemIndex() {
	return m_ListIndex + m_MaxItems - 1;
}
*/

Item* Window::GetSelectedItem() {
	if (m_SelectedIndex < 0 || m_SelectedIndex >= (int)m_Items.size()) return nullptr;
	return m_Items[m_SelectedIndex];
}

/*
int Window::GetNextSelectableItemIndex(int at, int by) {
	//if (m_SelectedIndex < 0 || m_SelectedIndex >= (int)m_Items.size()) return nullptr;

	int find = at + by;

	while (find >= 0 && find < (int)m_Items.size())
	{
		if (m_Items[find]->m_CanBeSelected) {
			return find;
		}
		find += by;
	}

	return at;
}
*/

int Window::FindSelectableItemIndex(int at, FIND_INDEX_DIR dir, FIND_INDEX_TYPE type)
{
	int addBy = dir == FIND_INDEX_DIR::UP ? -1 : 1;
	int index = at + addBy;

	int lastOption = at;

	while (index >= 0 && index < (int)m_Items.size())
	{
		if (m_Items[index]->m_CanBeSelected)
		{
			if (type == FIND_INDEX_TYPE::FIRST)
			{
				return index;
			}

			lastOption = index;
		}
		index += addBy;
	}
	return lastOption;
}

int Window::GetBottomMostIndex()
{
	return FindSelectableItemIndex(m_SelectedIndex, FIND_INDEX_DIR::DOWN, FIND_INDEX_TYPE::LAST);
}

int Window::GetTopMostIndex()
{
	return FindSelectableItemIndex(m_SelectedIndex, FIND_INDEX_DIR::UP, FIND_INDEX_TYPE::LAST);
}


Item* Window::AddItem(Item* item) {
	item->m_Size.x = m_Size.x;
	m_Items.push_back(item);
	return item;
}

Item* Window::AddItem(std::string text) {
	auto item = AddItem(new Item());
	item->m_Label = text;
	return item;
}

Item* Window::AddButton(std::string text) {
	auto item = AddItem(new Item());
	item->m_Label = text;
	item->m_CanBeSelected = true;
	return item;
}

CheckBox* Window::AddCheckBox(std::string text, bool* value) {
	auto checkBox = (CheckBox*)AddItem((Item*)new CheckBox(value));
	checkBox->m_Label = text;
	return checkBox;
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

Item* Window::AddDivider() {
	auto divider = AddItem(new Item());
	divider->m_Label = "";
	//divider->m_BackgroundColor = CRGBA(255, 255, 255, 255);
	return divider;
}

Item* Window::AddDivider(float height) {
	auto divider = AddDivider();
	divider->m_Size.y = height;
	return divider;
}

ButtonKey* Window::AddButtonKey(std::string text, Keybind* keybind) {
	auto buttonKey = (ButtonKey*)AddItem(new ButtonKey(keybind));
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