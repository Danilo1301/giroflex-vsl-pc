#pragma once

#include "pch.h"
#include "LightGroup.h"
#include "ColorTexture.h"

#include "menu/Window.h"

class Mod {
private:
    static void CreateMenu_Main();

    static void CreateMenu_EditLightGroups();
    static void CreateMenu_EditColorTextures();
    static void CreateMenu_EditPatterns();

    static void CreateMenu_EditLightGroup(LightGroup* lightGroup);
    static void CreateMenu_EditLightGroup_EditPatterns(LightGroup* lightGroup);
    static void CreateMenu_EditLightGroup_Points(LightGroup* lightGroup);
    static void CreateMenu_EditLightGroup_Point(LightGroup* lightGroup, Point* point);

    static void CreateMenu_EditColorTexture(ColorTexture* colorTexture);
    static void CreateMenu_EditColorTexture_EditPatterns(ColorTexture* colorTexture);

    static void CreateMenu_SelectPatterns_Select(std::function<void(PatternData*)>onConfirm, std::function<void(void)> onCancel); //make this return PatternData*
    static void CreateMenu_SelectPatterns_Edit(PatternData* patternData, std::function<void(bool)> onClose); //make this return PatternData*

    static void CreateMenu_EditPattern(Pattern* pattern);
    static void CreateMenu_EditPatternStep(Pattern* pattern, PatternStep* step);

    static void ToggleMenu();
public:
    static bool m_MainMenuVisible;
    static bool m_ShowDebug;
    static std::string m_MenuTitle;

    static void Update();
    static void Draw();
    static void SetPlayerControls(bool enabled);

    static void SaveToJSON();
    static void LoadFromJSON();
    static void ResetConfig();

    Mod();
};
