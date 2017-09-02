#pragma once

// Implements a WASD style movement controller
// Movement speed as world units per second
const float MOVE_SPEED = 20.0f;

const int CTRL_FORWARD = 1;
const int CTRL_BACK = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;

class Touch;

class MovementController : public IPointerHandler, public IKeyboardHandler
{
	URHO3D_OBJECT(MovementController, IPointerHandler)
public:
	MovementController(Context* context, SharedPtr<Node> object, float yaw, float pitch);

	void SetObject(SharedPtr<Node> newObject);
	void OnUpdate(float timeStep);

	bool VOnPointerMove(const IntVector2& mousePos, const int radius);
	bool VOnPointerButtonDown(const IntVector2& mousePos, const int radius, const String& buttonName);
	bool VOnPointerButtonUp(const  IntVector2& mousePos, const int radius, const String& buttonName);

	inline bool VOnKeyDown(const int c);
	inline bool VOnKeyUp(const int c);

public:
    // Movement controls. Assigned by the main program each frame.
    Controls m_Controls;
    SharedPtr<Touch> m_Touch;
	// Orientation Controls
	float		m_Yaw;
	float		m_Pitch;

    SharedPtr<Node> m_Object;

    bool m_bIsTouchEnabled;
};

