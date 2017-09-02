#pragma once

#include <GameEngine/GameLogic/BaseGameLogic.h>

#include "Database/CornersDatabase.h"

class CheckerFactory;
class DeckInfo;
class CheckerInfo;
class CheckerPos;

enum PlayerType
{
	INVALID_PLAYER = -1,
	FIRST_PLAYER = 1,
	SECOND_PLAYER = 2
};

class CornersLogic : public BaseGameLogic
{
	URHO3D_OBJECT(CornersLogic, BaseGameLogic)

public:
    CornersLogic(Context* context);
   
public:
    virtual bool VInitialize();
    virtual void VShutdown();
    virtual void VOnUpdate(float timeStep);
    virtual void VChangeState(enum BaseGameState newState);

    virtual void VAddView(SharedPtr<IGameView> pView, NodeId actorId = INVALID_NODE_ID);
    inline void SetCurrentStep(PlayerType& playerType) { m_CurrentPlayerStep = playerType; }
 	inline const PlayerType& GetCurrentStep() const { return m_CurrentPlayerStep; }
	const Vector2& GetMaxFieldSize() const { return m_MaxFieldSize; }
	const Rect& GetFirstPlayerHouse() const { return m_FirstPlayerHouse; }
	const Rect& GetSecondPlayerHouse() const { return m_SecondPlayerHouse; }
	const PlayerData& GetFirstPlayer() { return m_pCornersDatabase->GetPlayerData(); }
	const PlayerData& GetSecondPlayer() { return m_pCornersDatabase->GetGuestData(); }

protected:
    // Override this function to do any game-specific loading.
    virtual bool VLoadGameDelegate(String pLevelData);

	void SetTestPositions();

    void TryPlayAgainGameDelegate(StringHash eventType, VariantMap& eventData);

private:
	bool InitEnvironment();
	bool InitPlayer(PlayerType playerType);
    DeckInfo* CreateEndDeck(PlayerType playerType);
    CheckerInfo* CreateChecker(String prefabPath, Node* parent, const String& id, const CheckerPos& pos, PlayerType playerType, int leftStep);
    // excepts vector is needed for recursion, do not look the same values, it is discard values in ecxepts vector.
    // result vecctor is needed for all possible move positions
    void GetAllPossibleTurns(DeckInfo* deck, const CheckerPos& checkerPos, PODVector<Vector2>& excepts, PODVector<Vector2>* result, bool isInitialPosition);
    PlayerType IsGameFinish(const PlayerType& playerType);
	bool IsNotInDeck(const Vector2& pos);
    void ResetGame();

	// Delegates
	void CheckerSelectedDelegate(StringHash eventType, VariantMap& eventData);
	void CircleSelectedDelegate(StringHash eventType, VariantMap& eventData);

private:
    DeckInfo* m_pCurrentDeckInfo;
    DeckInfo* m_pEndDeckInfo;

    HashMap<String, HashMap<String, Node*>*> m_CheckersByPlayers;

    HashMap<String, Node*> m_FirstCircleSelects;
    HashMap<String, Node*> m_SecondCircleSelects;
    PODVector<Vector2> m_ActiveCircleSelects;

    Rect m_FirstPlayerHouse; // Left Top, Right Bottom (Checker pos)
    Rect m_SecondPlayerHouse; // Left Top, Right Bottom (Checker pos)
    Vector2 m_MinFieldSize;
	Vector2 m_MaxFieldSize;	// x - Row; y - Column;
	Vector2 m_PlayersChessCount;	// x - First player; y - Second player

    PlayerType m_CurrentPlayerStep;

    bool m_bIsGameFinished;
    bool m_bIsGameTryToFinish;
    bool m_bIsFirstPlayerInitialized;
    bool m_bIsSecondPlayerInitialized;

    NodeId m_SelectedCircleId;

    CheckerFactory* m_pCheckerFactory;
    CornersDatabase* m_pCornersDatabase;
};
