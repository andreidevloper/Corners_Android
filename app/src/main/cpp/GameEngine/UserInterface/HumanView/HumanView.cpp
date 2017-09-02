#include "GameEngineStd.h"

#include "GameLogic/BaseGameLogic.h"
#include "HumanView.h"

#include "Audio/AudioSubsystem.h"

ScreenResolution HumanView::g_ScreenResolution = ScreenResolution ::LayoutUnknown;

HumanView::HumanView(Context *context, Renderer* renderer) : IGameView(context)
{
	m_pRenderer = renderer;
	m_pKeyboardHandler.Reset();
	m_pPointerHandler.Reset();
    m_ControlledNodeId = M_MAX_UNSIGNED;
    m_pControlledNode = nullptr;

    m_ScreenJoystickIndex = M_MAX_UNSIGNED;
    m_ScreenJoystickSettingsIndex = M_MAX_UNSIGNED;

    m_bIsGraphicsOptionsTouchVisible = false;

    if(g_ScreenResolution == ScreenResolution::LayoutUnknown)
    {
        g_ScreenResolution = LayoutUtility::GetLayout();
    }

    InitAudio();
}

void HumanView::InitAudio()
{
    if (!g_pAudio)
    {
        g_pAudio = new AudioSubsystem(context_);
    }

    if (!g_pAudio)
    {
        return;
    }

    if (!g_pAudio->VInitialize())
    {
        return;
    }

    return;
}

// ----------------------------------------------------------
// Initializtion, update, shutdown.
// ----------------------------------------------------------
void HumanView::VOnAttach(GameViewId vid, NodeId aid)
{
	m_ViewId = vid;
	m_ControlledNodeId = aid;
}

bool HumanView::VOnRestore()
{
    if (g_pApp->GetGameOptions()->IsTouchEnabled() && GetPlatform() == "Android" || GetPlatform() == "iOS")
    {
        // On mobile platform, enable touch by adding a screen joystick
        InitTouchInput();
    }

	for (ScreenElementList::Iterator i = m_ScreenElements.Begin(); i != m_ScreenElements.End(); ++i)
	{
		(*i)->VOnRestore();
	}

	return true;
}

bool HumanView::VOnLostDevice()
{
	for (ScreenElementList::Iterator i = m_ScreenElements.Begin(); i != m_ScreenElements.End(); ++i)
	{
		(*i)->VOnLostDevice();
	}

	return true;
}

void HumanView::VOnUpdate(float timeStep)
{
	// Run through the all GUI screen's elements and call VOnUpdate.
    // Some screen elements need to update every frame
	for (ScreenElementList::Iterator i = m_ScreenElements.Begin(); i != m_ScreenElements.End(); ++i)
	{
		(*i)->VOnUpdate(timeStep);
	}
}

bool HumanView::VOnMsgProc(AppMsg message)
{
    // First, send input message for GUI elements
	for (ScreenElementList::Iterator i = m_ScreenElements.Begin(); i != m_ScreenElements.End(); ++i)
	{
        // Send input message only for visible GUI elements
		if ((*i)->VIsVisible())
        {
			if ((*i)->VOnMsgProc(message))
			{
				return true;
			}
		}
	}

    // Second, handle input message for HUMAN
	bool result = false;
	switch (message.uEvent)
	{
		case APP_EVENT::KEY_DOWN:
		{
			if (m_pKeyboardHandler.NotNull())
			{
				result = m_pKeyboardHandler->VOnKeyDown(message.lParam.m_keycode);

                // Handle graphic options from screen joystick
                GraphicsOptionJoystickHandle(message.lParam.m_keycode);
            }
		}
		break;

		case APP_EVENT::KEY_UP:
		{
			if (m_pKeyboardHandler.NotNull())
			{
				result = m_pKeyboardHandler->VOnKeyUp(message.lParam.m_keycode);
			}
		}
		break;

        case APP_EVENT::TOUCH_BEGIN:
        {
            if (m_pPointerHandler.NotNull())
            {
                result = m_pPointerHandler->VOnPointerButtonDown(message.lParam.m_position, 1, "PointerLeft");
            }
        }
        break;

        case APP_EVENT::MOUSE_MOVE:
		{
			if (m_pPointerHandler.NotNull())
			{
				result = m_pPointerHandler->VOnPointerMove(IntVector2((int)message.lParam.m_delta.x_, (int)message.lParam.m_delta.y_), 1);
			}
		}
		break;

        case APP_EVENT::MOUSE_LBUTTONDOWN:
		{
			if (m_pPointerHandler.NotNull())
			{
				result = m_pPointerHandler->VOnPointerButtonDown(message.lParam.m_position, 1, "PointerLeft");
			}
		}
		break;

        case APP_EVENT::MOUSE_LBUTTONUP:
		{
			if (m_pPointerHandler.NotNull())
			{
				result = m_pPointerHandler->VOnPointerButtonUp(message.lParam.m_position, 1, "PointerLeft");
			}
		}
			break;

        case APP_EVENT::MOUSE_RBUTTONDOWN:
		{
			if (m_pPointerHandler.NotNull())
			{
				result = m_pPointerHandler->VOnPointerButtonDown(IntVector2((int)message.lParam.m_position.x_, (int)message.lParam.m_position.y_), 1, "PointerRight");
			}
		}
			break;

        case APP_EVENT::MOUSE_RBUTTONUP:
		{
			if (m_pPointerHandler.NotNull())
			{
				result = m_pPointerHandler->VOnPointerButtonUp(IntVector2((int)message.lParam.m_position.x_, (int)message.lParam.m_position.y_), 1, "PointerRight");
			}
		}
			break;

	default:
		return false;
	}
	return result;
}

void HumanView::VPushElement(SharedPtr<IScreenElement> pElement)
{
	m_ScreenElements.PushFront(pElement);
}

void HumanView::VRemoveElement(SharedPtr<IScreenElement> pElement)
{
	ScreenElementList::Iterator it = m_ScreenElements.Find(pElement);
	if (it->NotNull())
	{
		m_ScreenElements.Erase(it);
	}
}

bool HumanView::LoadGame(SharedPtr<File> level)
{
	// call the delegate method
	return VLoadGameDelegate(level);
}

bool HumanView::VLoadGameDelegate(SharedPtr<File> level)
{
	if (m_pRenderer)
	{
		// Set rendering options for scene
		m_pScene = g_pApp->GetGameLogic()->GetScene();

		// Create the camera (not included in the scene file)
		m_pControlledNode = m_pScene->GetChild("MainCamera");

		if (m_pControlledNode == nullptr)
		{
            m_pControlledNode = m_pScene->CreateChild("MainCamera");
            m_pControlledNode->CreateComponent<Camera>();

			// Set an initial position for the camera scene node above the plane
            m_pControlledNode->SetPosition(Vector3(0.0f, 2.0f, -10.0f));
		}

		m_pViewport = SharedPtr<Viewport>(new Viewport(context_, m_pScene, m_pControlledNode->GetComponent<Camera>()));

		m_pRenderer->SetViewport(1, m_pViewport);
	}

	return true;
}

// Movement and options joysticks initialization/handling functions
void HumanView::InitTouchInput()
{
    ResourceCache* pCache = g_pApp->GetResCache();
    Input* pInput = g_pApp->GetInput();
    g_pApp->SetTouchEnabled(true);
    XMLFile* layout = pCache->GetResource<XMLFile>("UI/ScreenJoystick_Samples.xml");

    // Initialize input size for patch string
    InitTouchInputSize();
    const String& patchString = GetScreenJoystickPatchString();
    if (!patchString.Empty())
    {
        // Patch the screen joystick layout further on demand
        SharedPtr<XMLFile> patchFile(new XMLFile(context_));
        if (patchFile->FromString(patchString))
        {
            layout->Patch(patchFile);
        }
    }

    m_ScreenJoystickIndex = pInput->AddScreenJoystick(layout, pCache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    pInput->SetScreenJoystickVisible(m_ScreenJoystickIndex, true);
}

void HumanView::InitTouchInputSize()
{
    // For patch string
    m_TouchInputSize = LayoutUtility::GetTouchInputSize().ToString();
    m_TouchInputFontSize = String(LayoutUtility::GetTouchFontSize());

    IntVector2 touchOffest = LayoutUtility::GetTouchOffsetSize();

    m_TouchInputPosButton10 = IntVector2(12.0f, touchOffest.y_).ToString();

    int currentOffetX = 12.0f + touchOffest.x_ ;
    m_TouchInputPosButton11 = IntVector2(currentOffetX, touchOffest.y_).ToString();

    currentOffetX += touchOffest.x_;
    m_TouchInputPosButton12 = IntVector2(currentOffetX, touchOffest.y_).ToString();

    currentOffetX += touchOffest.x_;
    m_TouchInputPosButton13 = IntVector2(currentOffetX, touchOffest.y_).ToString();
}

void HumanView::InitTouchInputSettingSize()
{
    IntVector2 touchOffest = LayoutUtility::GetTouchSettingOffsetSize();

    m_TouchInputSettingPosButton0 = IntVector2(-12.0f, touchOffest.y_).ToString();

    int currentOffetX = -12.0f + touchOffest.x_ ;
    m_TouchInputSettingPosButton1 = IntVector2(currentOffetX, touchOffest.y_).ToString();

    currentOffetX += touchOffest.x_;
    m_TouchInputSettingPosButton2 = IntVector2(currentOffetX, touchOffest.y_).ToString();

    currentOffetX += touchOffest.x_;
    m_TouchInputSettingPosButton3 = IntVector2(currentOffetX, touchOffest.y_).ToString();


    touchOffest.y_ += touchOffest.y_;
    m_TouchInputSettingPosButton4 = IntVector2(-12.0f, touchOffest.y_).ToString();

    currentOffetX = -12.0f;
    currentOffetX += touchOffest.x_;
    m_TouchInputSettingPosButton5 = IntVector2(currentOffetX, touchOffest.y_).ToString();

    currentOffetX += touchOffest.x_;
    m_TouchInputSettingPosButton6 = IntVector2(currentOffetX, touchOffest.y_).ToString();

    currentOffetX += touchOffest.x_;
    m_TouchInputSettingPosButton7 = IntVector2(currentOffetX, touchOffest.y_).ToString();
}

void HumanView::InitGraphicOptionsInput()
{
    if(g_pApp->IsTouchEnabled())
    {
        ResourceCache* pCache = g_pApp->GetResCache();
        Input* pInput = g_pApp->GetInput();

        XMLFile* layout = pCache->GetResource<XMLFile>("UI/ScreenJoystickSettings_Samples.xml");
        SharedPtr<XMLFile> patchFile(new XMLFile(context_));

        InitTouchInputSettingSize();

        const String& patchString = GetScreenJoystickSettingPatchString();
        if (patchFile->FromString(patchString))
        {
            layout->Patch(patchFile);
        }

        m_ScreenJoystickSettingsIndex = pInput->AddScreenJoystick(layout, pCache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    }
}

void HumanView::GraphicsOptionJoystickHandle(int key)
{
    using namespace KeyDown;

    // Toggle console with F1
    if (key == KEY_F1)
        GetSubsystem<Console>()->Toggle();

        // Toggle debug HUD with F2
    else if (key == KEY_F2)
        GetSubsystem<DebugHud>()->ToggleAll();

        // Common rendering quality controls, only when UI has no focused element
    else if (!GetSubsystem<UI>()->GetFocusElement())
    {
        Renderer* renderer = GetSubsystem<Renderer>();

        // Graphics options Show/Hide
        if (key == KEY_SELECT)
        {
            if(m_ScreenJoystickSettingsIndex == M_MAX_UNSIGNED)
            {
                InitGraphicOptionsInput();
            }
            else
            {
                Input* pInput = g_pApp->GetInput();
                m_bIsGraphicsOptionsTouchVisible = !m_bIsGraphicsOptionsTouchVisible;
                pInput->SetScreenJoystickVisible(m_ScreenJoystickSettingsIndex, m_bIsGraphicsOptionsTouchVisible);
            }
        }

            // Texture quality
        else if (key == '1')
        {
            int quality = renderer->GetTextureQuality();
            ++quality;
            if (quality > QUALITY_HIGH)
                quality = QUALITY_LOW;
            renderer->SetTextureQuality(quality);
        }

            // Material quality
        else if (key == '2')
        {
            int quality = renderer->GetMaterialQuality();
            ++quality;
            if (quality > QUALITY_HIGH)
                quality = QUALITY_LOW;
            renderer->SetMaterialQuality(quality);
        }

            // Specular lighting
        else if (key == '3')
            renderer->SetSpecularLighting(!renderer->GetSpecularLighting());

            // Shadow rendering
        else if (key == '4')
            renderer->SetDrawShadows(!renderer->GetDrawShadows());

            // Shadow map resolution
        else if (key == '5')
        {
            int shadowMapSize = renderer->GetShadowMapSize();
            shadowMapSize *= 2;
            if (shadowMapSize > 2048)
                shadowMapSize = 512;
            renderer->SetShadowMapSize(shadowMapSize);
        }

            // Shadow depth and filtering quality
        else if (key == '6')
        {
            ShadowQuality quality = renderer->GetShadowQuality();
            quality = (ShadowQuality)(quality + 1);
            if (quality > SHADOWQUALITY_BLUR_VSM)
                quality = SHADOWQUALITY_SIMPLE_16BIT;
            renderer->SetShadowQuality(quality);
        }

            // Occlusion culling
        else if (key == '7')
        {
            bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
            occlusion = !occlusion;
            renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
        }

            // Instancing
        else if (key == '8')
            renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());

            // Take screenshot
        else if (key == '9')
        {
            Graphics* graphics = GetSubsystem<Graphics>();
            Image screenshot(context_);
            graphics->TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
                               Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        }
    }
}


