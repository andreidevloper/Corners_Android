// Add AlphaEngine base
#include "GameEngineStd.h"

// Add LayoutUtil
#include "LayoutUtility.h"

float LayoutUtility::g_TextFontSize = 10.0f;
float LayoutUtility::g_LineEditFontSize = 12.5f;
IntVector2 LayoutUtility::g_TouchInputSize = IntVector2(96, 96);
IntVector2 LayoutUtility::g_TouchOffsetSize = IntVector2(148, -76);
IntVector2 LayoutUtility::g_TouchSettingOffsetSize = IntVector2(-108, 84);
int LayoutUtility::g_TouchFontSize = 12.5f;
IntVector2 LayoutUtility::g_MainMenuWindowSize = IntVector2(400, 300);

// Layout utility class offers static function
// Which can be used by any BaseUI class
// Utility function helps to determine device screen resolution
// And set necessary font sizes for text box/line edit GUI elements
// Depends on screen resolution
ScreenResolution LayoutUtility:: GetLayout()
{
    // Get current windows dimensions
    IntVector2 screenSize = g_pApp->GetCurrentWindowSize();
    ScreenResolution result;

    if(screenSize.x_ <= 320)
    {
        g_TextFontSize = 6.0f;
        g_LineEditFontSize = 8.5f;

        g_TouchFontSize = 6;
        g_TouchInputSize = IntVector2(48, 48);
        g_TouchOffsetSize = IntVector2(52, -38);
        g_TouchSettingOffsetSize = IntVector2(-54, 44);

        g_MainMenuWindowSize = IntVector2(100, 100);
        result = LayoutSmall;
    }
    else if(screenSize.x_ <= 480)
    {
        g_TextFontSize = 8.0f;
        g_LineEditFontSize = 12.5f;

        g_TouchFontSize = 8;
        g_TouchInputSize = IntVector2(72, 72);
        g_TouchOffsetSize = IntVector2(70, -40);
        g_TouchSettingOffsetSize = IntVector2(-70, 70);

        g_MainMenuWindowSize = IntVector2(150, 150);
        result = LayoutNormal;
    }
    else if(screenSize.x_ <= 800)
    {
        g_TextFontSize = 15.0f;
        g_LineEditFontSize = 12.5f;

        g_TouchFontSize = 13;
        g_TouchInputSize = IntVector2(96, 96);
        g_TouchOffsetSize = IntVector2(105, -76);
        g_TouchSettingOffsetSize = IntVector2(-108, 92);

        g_MainMenuWindowSize = IntVector2(320, 220);
        result = LayoutLarge;
    }
    else
    {
        g_TextFontSize = 20.0f;
        g_LineEditFontSize = 25.0f;

        g_TouchFontSize = 15;
        g_TouchInputSize = IntVector2(144, 144);
        g_TouchOffsetSize = IntVector2(148, -76);
        g_TouchSettingOffsetSize = IntVector2(-160, 140);

        g_MainMenuWindowSize = IntVector2(400, 300);
        result = LayoutExtraLarge;
    }

    return result;
}
