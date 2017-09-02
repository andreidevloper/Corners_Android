#include "GameEngineStd.h"
#include "MovementController.h"
#include "Touch.h"

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

const float TOUCH_SENSITIVITY = 2.0f;

MovementController::MovementController(Context* context, SharedPtr<Node> object, float yaw, float pitch) : IPointerHandler(context), IKeyboardHandler(context), Object(context), m_Object(object)
{
    m_bIsTouchEnabled = g_pApp->IsTouchEnabled();
    if (m_bIsTouchEnabled)
    {
        m_Touch = new Touch(context_, TOUCH_SENSITIVITY);
    }
    m_Controls.yaw_ = yaw;
    m_Controls.pitch_ = pitch;
}

void MovementController::SetObject(SharedPtr<Node> newObject)
{
	m_Object = newObject;
}

bool MovementController::VOnKeyDown(const int c)
{
	return true;
}

bool MovementController::VOnKeyUp(const int c)
{
	return true;
}


void MovementController::OnUpdate(float timeStep)
{
    Input* input = GetSubsystem<Input>();
	if (GetSubsystem<UI>()->GetFocusElement() || GetSubsystem<UI>()->IsDragging())
		return;

    m_Controls.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT, false);

    // Update controls using touch utility class (e.x. screen touches or gyroscope)
    if (m_Touch)
    {
        m_Touch->UpdateTouches(m_Controls);
    }

    if (!m_Touch || !m_Touch->useGyroscope_)
    {
        m_Controls.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
        m_Controls.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
        m_Controls.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
        m_Controls.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
    }

    if (m_bIsTouchEnabled)
    {
        for (unsigned i = 0; i < input->GetNumTouches(); ++i)
        {
            TouchState* state = input->GetTouch(i);
            if (!state->touchedElement_)    // Touch on empty space
            {
                Camera* camera = m_Object->GetComponent<Camera>();
                if (!camera)
                    return;

                Graphics* graphics = GetSubsystem<Graphics>();
                m_Controls.yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
                m_Controls.pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;
            }
        }
    }

    if (m_Controls.IsDown(CTRL_FORWARD))
    {
        m_Object->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    }

    if (m_Controls.IsDown(CTRL_BACK))
    {
        m_Object->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    }

    if (m_Controls.IsDown(CTRL_LEFT))
    {
        m_Object->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    }

    if (m_Controls.IsDown(CTRL_RIGHT))
    {
        m_Object->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
    }

    m_Object->SetRotation(Quaternion(m_Controls.pitch_, m_Controls.yaw_, 0.0f));

    // Turn on/off gyroscope on mobile platform
    if (m_Touch && input->GetKeyPress(KEY_G))
        m_Touch->useGyroscope_ = !m_Touch->useGyroscope_;
}

bool MovementController::VOnPointerMove(const IntVector2& mousePos, const int radius)
{

	return true;
}

bool MovementController::VOnPointerButtonDown(const IntVector2& mousePos, const int radius, const String& buttonName)
{

	return false;
}

bool MovementController::VOnPointerButtonUp(const  IntVector2& mousePos, const int radius, const String& buttonName)
{

	return false;
}
