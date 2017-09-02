#include "CornersStd.h"
#include "CheckerController.h"

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

//---------------------------------------------------------------------------------------------------------------------
// LOCAL_EVENT_MOUSE_CHECKER_SELECTED- Checker controller sent event to CheckerLogic about selected checker
//---------------------------------------------------------------------------------------------------------------------


CheckerController::CheckerController(Context* context, CornersLogic* cornerLogic,
                                     PlayerType playerType, Scene* scene, Camera* camera)
		  : IPointerHandler(context), Object(context)
{
	memset(m_bIsKey, 0x00, sizeof(m_bIsKey));
	m_pCamera = camera;
	m_pScene = scene;
	m_PlayerType = playerType;
	m_pCornerLogic = cornerLogic;
}


void CheckerController::OnUpdate(float timeStep)
{

}

bool CheckerController::VOnPointerMove(const IntVector2& mousePos, const int radius)
{
	return false;
}

bool CheckerController::VOnPointerButtonDown(const IntVector2& mousePos, const int radius,
                                             const String& buttonName)
{
	const PlayerType& currentPlayerStep = m_pCornerLogic->GetCurrentStep();
	if (buttonName == "PointerLeft")
	{
		// Get Input System
		Input* pInput = GetSubsystem<Input>();
		Graphics* pGraphics = g_pApp->GetGraphics();

		// Ignore if mouse is grabbed by other operation
		if (pInput->IsMouseGrabbed())
		{
			return false;
		}

		// TODO: Need to test mousePos on Win/Linux OS
#ifndef  ANDROID
		IntVector2 pos = pInput->GetMousePosition();
#else
		IntVector2 pos = mousePos;
#endif

		// Calculate Normalized Position
		float posx = float((float) pos.x_ / (float) pGraphics->GetWidth());
		float posy = float((float) pos.y_ / (float) pGraphics->GetHeight());

		// If Camera is created return
		if (!m_pCamera)
		{
			return false;
		}

		// Get cast ray
		Ray cameraRay = m_pCamera->GetScreenRay(posx, posy);

		// Pointer to Selected componet
		Component* selectedComponent = NULL;

		// Phyics based
		PhysicsRaycastResult result;
		m_pScene->GetComponent<PhysicsWorld>()->RaycastSingle(result, cameraRay,
		                                                      m_pCamera->GetFarClip(), 1);

		// If a RigidBody is found
		if (result.body_ != NULL)
		{
			RigidBody* body = result.body_;
			selectedComponent = body;
		}
		else
		{
			//g_pApp->SetCursorShape("Main");
		}

		if (selectedComponent)
		{
			Node* pThisNode = selectedComponent->GetNode();
			PlayerType playerType = (PlayerType)pThisNode->GetVar("Player").GetInt();
			if(playerType == currentPlayerStep)
			{
				if (pThisNode)
				{
					String nodeName = pThisNode->GetName();
					if (nodeName.Contains("Checker"))
					{
						VariantMap data;
						data[MOUSE_CHECKER_SELECTED::P_NODE_ID] = pThisNode->GetID();
						data[MOUSE_CHECKER_SELECTED::P_SCENE] = m_pScene;
						SendEvent(LOCAL_EVENT_MOUSE_CHECKER_SELECTED, data);
					}
					else if (nodeName.Contains("CircleSelect"))
					{
						VariantMap data;
						data[MOUSE_CHECKER_SELECTED::P_NODE_ID] = pThisNode->GetID();
						data[MOUSE_CHECKER_SELECTED::P_SCENE] = m_pScene;
						SendEvent(LOCAL_EVENT_MOUSE_CIRCLE_SELECTED, data);
					}
				}
			}
		}

		URHO3D_LOGINFO(m_LastMousePos.ToString());

		m_LastMousePos = mousePos;
		return true;
	}
	return false;
}

bool CheckerController::VOnPointerButtonUp(const IntVector2& mousePos, const int radius,
                                           const String& buttonName)
{
	if (buttonName == "PointerLeft")
	{

		return true;
	}
	return false;
}


