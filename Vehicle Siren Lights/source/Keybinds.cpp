#include "Keybinds.h"

Keybind Keybinds::OpenMenu("L", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::ToggleDebug("D", KEYBIND_FLAGS::CTRL | KEYBIND_FLAGS::SHIFT);
Keybind Keybinds::ToggleLights("J", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::EditorUpDown("", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::EditorSlower("", KEYBIND_FLAGS::ALT);
Keybind Keybinds::ReloadConfig("R", KEYBIND_FLAGS::CTRL);
