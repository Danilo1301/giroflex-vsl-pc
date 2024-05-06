#include "TipMessage.h"

#include "../localization/Localization.h"
#include "../input/Input.h"
#include "../TestHelper.h"

bool TipMessage::m_Visible = false;
std::string TipMessage::m_Message = "TIP: message";
unsigned int TipMessage::m_StartShowMessageTime = 0;
unsigned int TipMessage::m_ShowMessageTime = 6000;

std::vector<std::string> idsShown;

void TipMessage::Show(std::string id, std::string message)
{
	if (std::find(idsShown.begin(), idsShown.end(), id) != idsShown.end())
	{
		//TestHelper::AddLine("Tip '" + id + "' already shown");
		return;
	}

	TestHelper::AddLine("Showing tip '" + id + "'");

	m_Visible = true;
	m_Message = message;
	m_StartShowMessageTime = CTimer::m_snTimeInMilliseconds;

	idsShown.push_back(id);
}

void TipMessage::ShowTip_MenuSelectKey()
{
	TipMessage::Show("menu_select", Localization::GetLine("tip_menu_select"));
}

void TipMessage::ShowTip_MenuBackKey()
{
	TipMessage::Show("menu_back", Localization::GetLine("tip_menu_back"));
}

void TipMessage::ShowTip_NumberRangeSetValue()
{
	TipMessage::Show("number_range_set_value", Localization::GetLine("tip_number_range_set_value"));
}


void TipMessage::Hide()
{
	m_Visible = false;
}

void TipMessage::Update()
{
	if (m_Visible)
	{
		if (Input::GetKeyDown(VK_ESCAPE) || Input::GetKeyDown(VK_SPACE)) //ESC //SPACE
		{
			Hide();
		}

		if (CTimer::m_snTimeInMilliseconds > (m_StartShowMessageTime + m_ShowMessageTime))
		{
			Hide();
		}
	}
}

void TipMessage::Draw()
{
	CVector2D size = CVector2D(400, 40);
	CVector2D position = CVector2D(
		1024.0f / 2 - size.x / 2,
		768.0f - size.y - 80.0f
	);
	CRGBA textColor = CRGBA(255, 255, 255);
	CRGBA bgColor = CRGBA(0, 0, 0, 190);

	Menu::DrawRect(position.x, position.y, size.x, size.y, bgColor);

	Menu::DrawString(m_Message, position.x + 10.0f, position.y + 10.0f, textColor);
	//Menu::DrawString(std::to_string(CTimer::m_snTimeInMilliseconds) + " / " + std::to_string((m_StartShowMessageTime + m_ShowMessageTime)), position.x + 10.0f, position.y + 30.0f, textColor);
}