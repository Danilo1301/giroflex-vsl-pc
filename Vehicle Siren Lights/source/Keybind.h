#pragma once

#include "pch.h"

#include "input/Input.h"

static std::map<int, std::string> KeyMaps = {
    {219, "(´)"},
    {222, "(~)"},
    {190, "."},
    {188, ","},
    {191, ";"},
    {193, "/"},
    {226, "\\"},
    {187, "="},
    {189, "-"},
    {192, "'"},
    {220, "]"},
    {221, "["}
};

enum KEYBIND_FLAGS : int
{
    NONE = 1 << 0,
    CTRL = 1 << 1,
    SHIFT = 1 << 2,
    ALT = 1 << 3
};

struct Keybind {
    bool SaveAsNumber = false;
    int Flags;
    unsigned int KeyCode;

    Keybind(std::string key, unsigned int flags)
    {
        SetKey(key, flags);
    }

    void SetKey(std::string key)
    {
        if (key.empty())
        {
            KeyCode = -1;
        }
        else {
            KeyCode = (int)key.at(0);

            for (auto k : KeyMaps)
            {
                if (k.second.compare(key) == 0)
                {
                    KeyCode = k.first;
                    break;
                }
            }
        }
    }

    void SetKey(std::string key, int flags)
    {
        Flags = flags;
        SetKey(key);
    }

    std::string GetKeybindString()
    {
        std::vector<std::string> ks;

        if (Flags & KEYBIND_FLAGS::CTRL) ks.push_back("CTRL");
        if (Flags & KEYBIND_FLAGS::ALT) ks.push_back("ALT");
        if (Flags & KEYBIND_FLAGS::SHIFT) ks.push_back("SHIFT");

        if (KeyCode != -1) ks.push_back(GetKeyString());

        return join(ks, " + ");
    }

    std::string GetKeyString()
    {
        if (KeyCode == -1) return "";

        for (auto k : KeyMaps)
        {
            if (k.first == KeyCode) return k.second;
        }

        char ch = static_cast<char>(KeyCode);
        return ToUpper(std::string(1, ch));
    }

    bool CheckKeybind()
    {
        if ((Flags & KEYBIND_FLAGS::ALT) && !Input::GetKey(18)) return false;
        if ((Flags & KEYBIND_FLAGS::CTRL) && !Input::GetKey(17)) return false;
        if ((Flags & KEYBIND_FLAGS::SHIFT) && !Input::GetKey(16)) return false;

        if (KeyCode == -1)
        {
            if (Flags == 0) return false;
            return true;
        }

        return Input::GetKeyDown(KeyCode);
    }

    //change this
    Json::Value ToJSON()
    {
        Json::Value value = Json::objectValue;

        if (KeyCode == -1) {
            value["key"] = "";
        }
        else
        {
            if (SaveAsNumber) value["key"] = KeyCode;
            else value["key"] = GetKeyString();
        }

        value["alt"] = (Flags & KEYBIND_FLAGS::ALT) > 0;
        value["shift"] = (Flags & KEYBIND_FLAGS::SHIFT) > 0;
        value["ctrl"] = (Flags & KEYBIND_FLAGS::CTRL) > 0;

        return value;
    }

    //change this
    void FromJSON(Json::Value value)
    {
        if (value.empty()) return;

        SaveAsNumber = value["key"].isNumeric();

        if (SaveAsNumber)
        {
            KeyCode = value["key"].asInt();
        }
        else {
            auto keyStr = ToUpper(value["key"].asString());

            if (keyStr.empty()) KeyCode = -1;
            else KeyCode = (int)keyStr.at(0);
        }

        Flags = 0;
        if (value["alt"].asBool()) Flags |= KEYBIND_FLAGS::ALT;
        if (value["shift"].asBool()) Flags |= KEYBIND_FLAGS::SHIFT;
        if (value["ctrl"].asBool()) Flags |= KEYBIND_FLAGS::CTRL;
    }
};