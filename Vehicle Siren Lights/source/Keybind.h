#pragma once

#include "pch.h"

enum KEYBIND_FLAGS : int
{
    NONE = 1 << 0,
    CTRL = 1 << 1,
    SHIFT = 1 << 2,
    ALT = 1 << 3
};

struct Keybind {
    //bool code = false;
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

        if(KeyCode != -1) ks.push_back(GetKeyString());

        return join(ks, " + ");
    }

    std::string GetKeyString()
    {
        if (KeyCode == -1) return "";

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

    /*
    Keybind(int key, KEYBIND_FLAGS flags)
    {

    }
    */

    /*
    * Json::Value ToJSON()
    {
        Json::Value value = Json::objectValue;

        if (key == -1) {
            value["key"] = "";
        }
        else
        {
            if (code) value["key"] = key;
            else value["key"] = GetKeyString();
        }

        value["alt"] = alt;
        value["shift"] = shift;
        value["ctrl"] = ctrl;

        return value;
    }

    void FromJSON(Json::Value value)
    {
        if (value.empty()) return;

        code = value["key"].isNumeric();

        if (code)
        {
            key = value["key"].asInt();
        }
        else {
            auto keyStr = ToUpper(value["key"].asString());

            if (keyStr.empty()) key = -1;
            else key = (int)keyStr.at(0);
        }

        alt = value["alt"].asBool();
        shift = value["shift"].asBool();
        ctrl = value["ctrl"].asBool();
    }

    std::string GetKeybindString()
    {
        std::vector<std::string> ks;

        if (ctrl) ks.push_back("CTRL");
        if (shift) ks.push_back("SHIFT");
        if (alt) ks.push_back("ALT");

        if (ks.size() == 0) return GetKeyString();

        if(key != -1) ks.push_back(GetKeyString());

        return join(ks, " + ");
    }

    std::string GetKeyString()
    {
        if (key == -1) return "";

        char ch = static_cast<char>(key);
        return ToUpper(std::string(1, ch));
    }

    bool CheckKeybind()
    {
        if (alt && !Input::GetKey(18)) return false;
        if (ctrl && !Input::GetKey(17)) return false;
        if (shift && !Input::GetKey(16)) return false;

        if (key == -1) return true;

        return Input::GetKeyDown(key);
    }
    */
};