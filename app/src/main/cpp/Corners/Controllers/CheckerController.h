#pragma once

#include <GameLogic/CornersLogic.h>

const int CTRL_SELECT_CHECKER = 1;
const int CTRL_SELECT_MOVE = 2;

URHO3D_EVENT(LOCAL_EVENT_MOUSE_CHECKER_SELECTED, MOUSE_CHECKER_SELECTED)
{
	URHO3D_PARAM(P_SCENE, P_SCENE_VALUE);
	URHO3D_PARAM(P_NODE_ID, P_NODE_ID_VALUE);
}

URHO3D_EVENT(LOCAL_EVENT_MOUSE_CIRCLE_SELECTED, MOUSE_CIRCLE_SELECTED)
{
	URHO3D_PARAM(P_SCENE, P_SCENE_VALUE);
	URHO3D_PARAM(P_NODE_ID, P_NODE_ID_VALUE);
}

class CornersLogic;

// Implements a WASD style movement controller
class CheckerController : public IPointerHandler
{
	URHO3D_OBJECT(CheckerController, IKeyboardHandler)
public:
	CheckerController(Context* context, CornersLogic* cornersLogic, PlayerType playerType, Scene* scene, Camera* camera);
	
	void OnUpdate(float timeStep);

	virtual bool VOnPointerMove(const IntVector2& mousePos, const int radius);
	virtual bool VOnPointerButtonDown(const IntVector2& mousePos, const int radius, const String& buttonName);
	virtual bool VOnPointerButtonUp(const  IntVector2& mousePos, const int radius, const String& buttonName);

	inline bool VOnKeyDown(const int c) { if(c >= 0 && c <= 255) m_bIsKey[c] = true; return true; }
	inline bool VOnKeyUp(const int c) { if (c >= 0 && c <= 255) m_bIsKey[c] = false; return true; }

	inline bool VIsKeyDown(const int c) { return false; }
	inline bool VIsKeyUp(const int c) { return false; }

public:
	CornersLogic* m_pCornerLogic;

	IntVector2 m_LastMousePos;
	bool m_bIsKey[256];						// Which keys are up and down

	Scene* m_pScene;
	Camera* m_pCamera;

	PlayerType m_PlayerType;
};


