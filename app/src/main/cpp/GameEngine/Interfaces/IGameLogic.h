#pragma once

class IGamePhysics;

enum BaseGameState : int;

class IGameLogic : public Object
{
public:
	IGameLogic(Context* context);
	virtual ~IGameLogic();

	virtual bool VInitialize() = 0;
	virtual void VOnUpdate(float timeStep) = 0;
	virtual void VShutdown() = 0;

	virtual void VChangeState(BaseGameState newState) = 0;
	virtual bool VLoadGame(String levelResource) = 0;
};