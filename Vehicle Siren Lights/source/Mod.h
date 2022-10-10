#pragma once

#include "pch.h"

class Mod {
public:
    static std::string Version;
    static bool IsSamp;
    static bool DebugEnabled;

    Mod();

    static void Init();
    static void Update();
    static void Draw();

    static void SetPlayerControl(bool enabled);
    static void ToggleMenu();
    static void ToggleDebug();
    static void ReloadConfig();
    static void DisableOriginalLights();
};
