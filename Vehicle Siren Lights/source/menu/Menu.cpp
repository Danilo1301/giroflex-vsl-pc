#include "Menu.h"
#include "../input/Input.h"
#include "PositionEditor.h"
#include "TextEditor.h"

std::vector<Window*> Menu::m_Windows;
Window* Menu::m_ActiveWindow = nullptr;
Item* Menu::m_ItemClicked = nullptr;
bool Menu::m_Visible = false;
bool Menu::m_Hide = false;
int Menu::m_OpenAtIndex = -1;
CVector2D Menu::m_DefaultPosition = CVector2D(10.0f, 120.0f);
eFontAlignment Menu::m_FontAlign = eFontAlignment::ALIGN_LEFT;

void Menu::DrawRect(float x, float y, float width, float height, CRGBA color) {
	CSprite2d::DrawRect(CRect(ScreenX(x), ScreenY(y), ScreenX(x + width), ScreenY(y + height)), color);
}

void Menu::DrawString(std::string text, float x, float y, CRGBA color)
{
    char buffer[256];
    sprintf_s(buffer, "%s", text.c_str());

    CFont::m_fWrapx = 100000.0f;
    CFont::SetOrientation(m_FontAlign);
    CFont::SetDropShadowPosition(0);
    CFont::SetRightJustifyWrap(false);
    CFont::SetJustify(false);
    CFont::SetBackground(false, false);
    CFont::SetScale(ScreenX(0.35f), ScreenY(0.95f));
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetProportional(true);
    CFont::SetColor(color);
    CFont::PrintString(ScreenX(x), ScreenY(y), buffer);
}

void Menu::DrawArrow(float x, float y, eArrowDirection direction, float width, float height, CRGBA color)
{
    auto point1 = CVector2D(0, 0);
    auto point2 = CVector2D(0, 0);
    auto point3 = CVector2D(0, 0);

    auto translate = CVector2D(x, y);

    if (direction == eArrowDirection::LEFT || direction == eArrowDirection::RIGHT) {
        point2.x = width;
        point2.y = height/2;

        point3.y = height;

        if (direction == eArrowDirection::LEFT) {
            translate.x += point2.x;
            point2.x = -point2.x;
        }
    }

    if (direction == eArrowDirection::UP || direction == eArrowDirection::DOWN) {
        point2.x = width;
  
        point3.x = width / 2;
        point3.y = height;

        if(direction == eArrowDirection::UP) {
            translate.y += point3.y;
            point3.y = -point3.y;
        }
    }

    point1 += translate;
    point2 += translate;
    point3 += translate;

    CSprite2d::Draw2DPolygon(
        ScreenX(point1.x), ScreenY(point1.y),
        ScreenX(point2.x), ScreenY(point2.y),
        ScreenX(point3.x), ScreenY(point3.y),
        ScreenX(point1.x), ScreenY(point1.y),
        color);
}

void Menu::DrawArrowButton(float x, float y, eArrowDirection direction, float width, float height, bool active) {
    float padding = 5.0f;
    DrawRect(x, y, width, height, CRGBA(0, 0, 0));
    DrawArrow(x + padding, y + padding, direction, width - padding*2, height - padding * 2, active ? CRGBA(255, 255, 255) : CRGBA(64, 64, 64));
}

void Menu::DrawKeyButton(std::string text, float x, float y, float width, float height) {
    DrawRect(x, y, width, height, CRGBA(255, 255, 255));
    DrawRect(x + 2, y + 2, width - 4, height - 4, CRGBA(0, 0, 0));

    m_FontAlign = eFontAlignment::ALIGN_CENTER;
    Menu::DrawString(text, x + width / 2, y + height / 2 - 10.0f, CRGBA(255, 255, 255));
    m_FontAlign = eFontAlignment::ALIGN_LEFT;
}

void Menu::DrawArrowKeyButton(float x, float y, eArrowDirection direction, float width, float height) {
    float padding = 5.0f;
    DrawKeyButton("", x, y, width, height);
    DrawArrow(x + padding, y + padding, direction, width - padding * 2, height - padding * 2, CRGBA(255, 255, 255));
}

void Menu::DrawListArrowIndicator(float x, float y, eArrowDirection direction, float width, float height) {
    float arrW = 40.0f;

    Menu::DrawRect(x, y, width, height, CRGBA(120, 120, 120));
    Menu::DrawArrow(x + (width / 2) - arrW / 2, y + 4, direction, arrW, height - 8, CRGBA(255, 255, 255));
}

Window* Menu::AddWindow(std::string title, std::string description) {
    Window* window = new Window();
    window->m_Title = title;
    window->m_Description = description;
    window->m_Position = m_DefaultPosition;
    m_Windows.push_back(window);
    m_ActiveWindow = window;
    return window;
}

Window* Menu::AddWindow(std::string title) {
    return AddWindow(title, "");
}

Window* Menu::AddWindow() {
    return AddWindow("", "");
}

void Menu::RemoveWindow(Window* window) {
    auto it = std::find(m_Windows.begin(), m_Windows.end(), window);
    if (it == m_Windows.end()) return;

    if (m_ActiveWindow == window) m_ActiveWindow = nullptr;

    window->Destroy();
    m_Windows.erase(it);
    delete window;
}

void Menu::RemoveAllWindows() {
    while (m_Windows.size() > 0) {
        RemoveWindow(m_Windows[0]);
    }
}

Window* Menu::CreateColorPickerWindow(CRGBA* color, std::function<void(void)> onClose) {
    auto window = Menu::AddWindow();
    window->m_Position.x += window->m_Size.x + 5.0f;
    window->m_Size.x = 250;

    window->AddNumberRange<unsigned char>("Red", &color->r, 0, 255);
    window->AddNumberRange<unsigned char>("Green", &color->g, 0, 255);
    window->AddNumberRange<unsigned char>("Blue", &color->b, 0, 255);
    window->AddNumberRange<unsigned char>("Alpha", &color->a, 0, 255);

    auto confirmButton = window->AddButton("Confirm");
    confirmButton->m_OnClick = [window, onClose]() {
        Menu::RemoveWindow(window);
        onClose();
    };

    return window;
}

void Menu::SetOpen(bool open) {
    m_Visible = open;
    m_OpenAtIndex = -1;

    if (!m_Visible && PositionEditor::m_Visible) PositionEditor::Toggle(nullptr);
}

void Menu::Draw() {
    if (m_Hide) return;

    m_FontAlign = eFontAlignment::ALIGN_LEFT;

    if (m_Visible && !PositionEditor::m_Visible) {
        for (Window* window : m_Windows)
        {
            window->Draw();
        }
    }

    if(PositionEditor::m_Visible) PositionEditor::Draw();
    if(TextEditor::m_Visible) TextEditor::Draw();
}

void Menu::Update() {

    PositionEditor::Update();
    TextEditor::Update();

    if (ButtonKey::m_EditingButtonKey) {
        ButtonKey::m_EditingButtonKey->Update();
        return;
    }

    if (PositionEditor::m_Visible) return;
    if (TextEditor::m_Visible) return;
    

    if (m_Visible) {
        if (Input::GetKeyDown(VK_UP)) {
            if (m_ActiveWindow) m_ActiveWindow->GoUp();
        }

        if (Input::GetKeyDown(VK_DOWN)) {
            if (m_ActiveWindow) m_ActiveWindow->GoDown();
        }
    }

    for (Window* window : m_Windows)
    {
        window->Update();
    }

    if (m_ItemClicked) {
        auto fn = m_ItemClicked->m_OnClick;
        m_ItemClicked = nullptr;

        fn();
    }


    if (TextEditor::m_OnConfirm != NULL && !TextEditor::m_Visible) {
        TextEditor::m_OnConfirm();
        TextEditor::m_OnConfirm = NULL;
    }
}

void Menu::SaveIndex() {
    m_OpenAtIndex = m_ActiveWindow->m_SelectedIndex;
}

void Menu::RestoreIndex() {
    m_ActiveWindow->m_SelectedIndex = m_OpenAtIndex;
    m_OpenAtIndex = -1;
    
}