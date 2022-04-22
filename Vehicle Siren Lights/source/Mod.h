#pragma once

#include "pch.h"

class Mod {
private:
public:
    static bool m_DebugEnabled;
    static bool m_IsSamp;

    static void Update();
    static void Draw();

    static void SetPlayerControl(bool enabled);
    static void ToggleMenu();

    Mod();
};
