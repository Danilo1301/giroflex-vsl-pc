#pragma once

#include "Menu.h"

class TipMessage {
public:
	static bool m_Visible;
	static std::string m_Message;
	static unsigned int m_StartShowMessageTime;
	static unsigned int m_ShowMessageTime;
	
	static void Show(std::string id, std::string message);
	static void ShowTip_MenuSelectKey();
	static void ShowTip_MenuBackKey();
	static void ShowTip_NumberRangeSetValue();
	static void Hide();
	static void Update();
	static void Draw();
};