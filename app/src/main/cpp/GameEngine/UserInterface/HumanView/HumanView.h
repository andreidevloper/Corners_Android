#pragma once

#include "../LayoutUtility.h"

// Human view class implements IGameView interface
// In order to show 2D/3D world for HUMAN and
// Ensure USER INTERFACE for HUMAN
// Human view class is derrived in new classes, e.x. in magnet application's game view layer "MagnetView",
// In order to show 2D/3D world of Magnet application and
// Ensure USER INTEFACE for operator that run magnet application
class HumanView : public IGameView
{
	URHO3D_OBJECT(HumanView, IGameView)
public:
	HumanView(Context *context, Renderer* renderer);
	// Attach view id and node id of scene's object, if
    // HUMAN can control camera or specific simulation's object.
	// E.x. MagnetView controls object with name MainCamera that has CameraComponent for 3D scene
    // E.x. Main menu view doesn't have any actors.
	virtual void VOnAttach(GameViewId vid, NodeId aid);

    // Look BaseUI class
	virtual bool VOnRestore();
    // Look BaseUI class
	virtual bool VOnLostDevice();
    // Look BaseUI class
	virtual void VOnUpdate(float timeStep);

	// Handle input message from devices
	virtual bool VOnMsgProc(AppMsg message);
    virtual void VShutdown() { }

	// Get type from game view. Can be different, depend on realization.
	virtual GameViewType VGetType() { return GameView_Human; }
	// Get unique id from game view. 
	virtual GameViewId VGetId() const { return m_ViewId; }

	// Virtual methods control the layering of interface elements
    // HumanView class helps to save UI screen elements in ScreenElementList container
	virtual void VPushElement(SharedPtr<IScreenElement> pElement);
	virtual void VRemoveElement(SharedPtr<IScreenElement> pElement);

    // This function is called by BaseGameLogic during game loading
	bool LoadGame(SharedPtr<File> level);

protected:
    virtual bool VLoadGameDelegate(SharedPtr<File> level);

	void InitAudio();

protected:

#pragma region Joystick for Android platform
	/// Return XML patch instructions for screen joystick layout for simulation app
	int GetScreenJoystickSettingsIndex() { return m_ScreenJoystickSettingsIndex; }
	void SetScreenJoystickSettingsIndex(int index) { m_ScreenJoystickSettingsIndex = index; }

	/// Initialize touch input on mobile platform for HUMAN.
	void InitTouchInput();
	void InitGraphicOptionsInput();
	/// Initialize joystick for graphics options based on selected option
	void GraphicsOptionJoystickHandle(int key);

private:
    void InitTouchInputSize();
    void InitTouchInputSettingSize();
    virtual String GetScreenJoystickPatchString() const {
        return
                "<patch>"
                        "    <add sel=\"/element\">"
                        "        <element type=\"Button\">"
                        "            <attribute name=\"Name\" value=\"Button11\" />"
                        "            <attribute name=\"Position\" value=\"" + m_TouchInputPosButton11 + "\" />"
                        "            <attribute name=\"Size\" value=\"" + m_TouchInputSize + "\" />"
                        "            <attribute name=\"Horiz Alignment\" value=\"Left\" />"
                        "            <attribute name=\"Vert Alignment\" value=\"Bottom\" />"
                        "            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />"
                        "            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />"
                        "            <attribute name=\"Hover Image Offset\" value=\"0 0\" />"
                        "            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />"
                        "            <element type=\"Text\">"
                        "                <attribute name=\"Name\" value=\"Label\" />"
                        "                <attribute name=\"Horiz Alignment\" value=\"Center\" />"
                        "                <attribute name=\"Vert Alignment\" value=\"Center\" />"
                        "                <attribute name=\"Color\" value=\"0 0 0 1\" />"
                        "                <attribute name=\"Text\" value=\"Gyroscope\" />"
                        "                <attribute name=\"Font Size\" value=\"" + m_TouchInputFontSize + "\" />"
                        "            </element>"
                        "            <element type=\"Text\">"
                        "                <attribute name=\"Name\" value=\"KeyBinding\" />"
                        "                <attribute name=\"Text\" value=\"G\" />"
                        "            </element>"
                        "        </element>"
                        "    </add>"

                        "    <add sel=\"/element\">"
                        "        <element type=\"Button\">"
                        "            <attribute name=\"Name\" value=\"Button12\" />"
                        "            <attribute name=\"Position\" value=\"" + m_TouchInputPosButton12 + "\" />"
                        "            <attribute name=\"Size\" value=\"" + m_TouchInputSize + "\" />"
                        "            <attribute name=\"Horiz Alignment\" value=\"Left\" />"
                        "            <attribute name=\"Vert Alignment\" value=\"Bottom\" />"
                        "            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />"
                        "            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />"
                        "            <attribute name=\"Hover Image Offset\" value=\"0 0\" />"
                        "            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />"
                        "            <element type=\"Text\">"
                        "                <attribute name=\"Name\" value=\"Label\" />"
                        "                <attribute name=\"Horiz Alignment\" value=\"Center\" />"
                        "                <attribute name=\"Vert Alignment\" value=\"Center\" />"
                        "                <attribute name=\"Color\" value=\"0 0 0 1\" />"
                        "                <attribute name=\"Text\" value=\"ON/OFF\" />"
                        "                <attribute name=\"Font Size\" value=\"" + m_TouchInputFontSize + "\" />"
                        "            </element>"
                        "            <element type=\"Text\">"
                        "                <attribute name=\"Name\" value=\"KeyBinding\" />"
                        "                <attribute name=\"Text\" value=\"O\" />"
                        "            </element>"
                        "        </element>"
                        "    </add>"

                        "    <add sel=\"/element\">"
                        "        <element type=\"Button\">"
                        "            <attribute name=\"Name\" value=\"Button13\" />"
                        "            <attribute name=\"Position\" value=\"" + m_TouchInputPosButton13 + "\" />"
                        "            <attribute name=\"Size\" value=\"" + m_TouchInputSize + "\" />"
                        "            <attribute name=\"Horiz Alignment\" value=\"Left\" />"
                        "            <attribute name=\"Vert Alignment\" value=\"Bottom\" />"
                        "            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />"
                        "            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />"
                        "            <attribute name=\"Hover Image Offset\" value=\"0 0\" />"
                        "            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />"
                        "            <element type=\"Text\">"
                        "                <attribute name=\"Name\" value=\"Label\" />"
                        "                <attribute name=\"Horiz Alignment\" value=\"Center\" />"
                        "                <attribute name=\"Vert Alignment\" value=\"Center\" />"
                        "                <attribute name=\"Color\" value=\"0 0 0 1\" />"
                        "                <attribute name=\"Text\" value=\"CW/CCW\" />"
                        "                <attribute name=\"Font Size\" value=\"" + m_TouchInputFontSize + "\" />"
                        "            </element>"
                        "            <element type=\"Text\">"
                        "                <attribute name=\"Name\" value=\"KeyBinding\" />"
                        "                <attribute name=\"Text\" value=\"P\" />"
                        "            </element>"
                        "        </element>"
                        "    </add>"

                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button2']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]/attribute[@name='Position']/@value\">" + m_TouchInputPosButton10 + "</replace>"
                "</patch>";

	}

    virtual String GetScreenJoystickSettingPatchString() const {

        return
                "<patch>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button2']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button3']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button4']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button5']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button6']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button7']]/attribute[@name='Size']/@value\">" + m_TouchInputSize + "</replace>"

                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton0 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton1 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button2']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton2 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button3']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton3 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button4']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton4 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button5']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton5 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button6']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton6 + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button7']]/attribute[@name='Position']/@value\">" + m_TouchInputSettingPosButton7 + "</replace>"

                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button2']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button3']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button4']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button5']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button6']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"
                        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button7']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Font Size']/@value\">" + m_TouchInputFontSize + "</replace>"

                "</patch>";

    }

    #pragma endregion

protected:
	GameViewId m_ViewId;							// Unique game view id.
	NodeId m_ControlledNodeId;						// Controlled node.
    Node* m_pControlledNode;

    Scene* m_pScene;								// Game scene from BaseGameLogic

    Renderer* m_pRenderer;							// 3D rendering capabilities, scene rendering.
    SharedPtr<Viewport> m_pViewport;				// Viewport

	ScreenElementList m_ScreenElements;				// Container for all UI elements

	SharedPtr<IPointerHandler> m_pPointerHandler;	// Can implement simulation behaviour for mouse moving or clicking
	SharedPtr<IKeyboardHandler> m_pKeyboardHandler; // Can implement simulation behaviour of keyboard's/joystick's key pressing

	// Screen joystick index for navigational controls (mobile platforms only)
	unsigned int m_ScreenJoystickIndex;
	// Screen joystick index for settings (mobile platforms only)
	unsigned int m_ScreenJoystickSettingsIndex;
	bool m_bIsGraphicsOptionsTouchVisible;

private:
    String m_TouchInputSize;
    String m_TouchInputFontSize;
    String m_TouchInputPosButton10;
    String m_TouchInputPosButton11;
    String m_TouchInputPosButton12;
    String m_TouchInputPosButton13;

    String m_TouchInputSettingPosButton0;
    String m_TouchInputSettingPosButton1;
    String m_TouchInputSettingPosButton2;
    String m_TouchInputSettingPosButton3;
    String m_TouchInputSettingPosButton4;
    String m_TouchInputSettingPosButton5;
    String m_TouchInputSettingPosButton6;
    String m_TouchInputSettingPosButton7;

    static ScreenResolution g_ScreenResolution;
};
