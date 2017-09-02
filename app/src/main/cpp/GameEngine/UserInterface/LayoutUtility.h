#pragma once

// resolution
enum ScreenResolution
{
    LayoutUnknown = -1,
    LayoutSmall = 0,
    LayoutNormal = 1,
    LayoutLarge = 2,
    LayoutExtraLarge = 3
};

// filename suffix based on resolution
static const char * ScreenPath[] = {"Small","Medium","Large","ExtraLarge"};

// Layout util
class LayoutUtility
{
    public:
    // Get Layout
    static ScreenResolution GetLayout();
    static float GetTextFontSize() { return g_TextFontSize; }
    static float GetLineEditFontSize() { return g_LineEditFontSize; }
    static IntVector2 GetTouchInputSize() { return g_TouchInputSize; }
    static IntVector2 GetTouchOffsetSize() { return g_TouchOffsetSize; }
    static IntVector2 GetTouchSettingOffsetSize() { return g_TouchSettingOffsetSize; }
    static IntVector2 GetMainMenuWindowSize() { return g_MainMenuWindowSize; }
    static int GetTouchFontSize() { return g_TouchFontSize; }

protected:
    static float g_TextFontSize;
    static float g_LineEditFontSize;
    static IntVector2 g_MainMenuWindowSize;
    static IntVector2 g_TouchInputSize;
    static IntVector2 g_TouchOffsetSize;
    static IntVector2 g_TouchSettingOffsetSize;
    static int g_TouchFontSize;
};
