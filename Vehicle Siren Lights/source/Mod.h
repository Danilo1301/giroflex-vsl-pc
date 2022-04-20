#pragma once

#include "pch.h"

class Mod {
private:
public:
    static int testInt;

    static void Update();
    static void Draw();

    static void SetPlayerControl(bool enabled);
    static void ToggleMenu();

    Mod();
};
