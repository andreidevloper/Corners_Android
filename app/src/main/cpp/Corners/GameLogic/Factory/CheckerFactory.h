#pragma once

#include "../CornersLogic.h"

enum CheckerMoveType
{
    INVALID = -1,
    STAY = 1,
    MOVE = 2,
    JUMP = 3,
    IN_HOUSE = 4,
    OUT_HOUSE = 5,
    IN_SECOND_PL_HOUSE = 6
};

class CheckerPos
{
public:
    CheckerPos()
    {
        m_Pos = Vector2(-1, -1);
    }

    CheckerPos(Vector2 pos)
    {
        m_Pos = pos;
    }

    CheckerPos(const CheckerPos& checker)
    {
        m_Pos = checker.m_Pos;
    }

    inline const Vector2& GetPos() const { return m_Pos; }

    // Calcualte to 3D pos
    static Matrix4 CalculateCheckerPos3D(const CheckerPos& checkerPos);
    // Calcualte from 3D pos
    static CheckerPos CalculateCheckedPosFrom3D(const Vector3& pos);

private:
    Vector2 m_Pos;
};

class CheckerInfo
{
public:
    CheckerInfo()
    {
        m_Id = "Invalid";
        m_LeftStep = -1;
        m_PlayerType = INVALID_PLAYER;
    }

    CheckerInfo(String id)
    {
        m_Id = id;
        m_LeftStep = -1;
        m_PlayerType = INVALID_PLAYER;
    }

    CheckerInfo(String id, CheckerPos checkerPos, int leftStep, PlayerType playerType, Node* pNode)
    {
        m_Id = id;
        m_Pos = checkerPos;
        m_LeftStep = leftStep;
        m_PlayerType = playerType;
        m_pNode = pNode;
    }

    inline const unsigned int GetId() const { return m_Id.ToHash(); }
    inline const CheckerPos& GetCheckerPos() const { return m_Pos; }
    inline const PlayerType& GetPlayerType() const { return m_PlayerType; }
    inline int GetLeftStep() const { return m_LeftStep; }
    inline Node* GetNode() const { return m_pNode; }

    inline void SetId(const String& id) { m_Id = id; }
    inline void SetCheckerPos(const CheckerPos& pos) { m_Pos = pos; }
    inline void SetLeftStep(const int leftStep) { m_LeftStep = leftStep; }
    inline void SetPlayerType(PlayerType playerType) { m_PlayerType = playerType; }
    inline void SetNode(Node* pNode) { m_pNode = pNode; }

private:
    String m_Id;
    CheckerPos m_Pos;
    int m_LeftStep;
    PlayerType m_PlayerType;
    Node* m_pNode;
};

typedef HashMap<String, CheckerInfo*> DeckByCheckerPos;
typedef HashMap<int, PODVector<CheckerInfo*>> DeckByPlayers;

class DeckInfo
{
public:
    DeckInfo()
    {
        m_Id = "Invalid";
        m_DeckByPlayers[FIRST_PLAYER] = PODVector<CheckerInfo*>(0);
        m_DeckByPlayers[SECOND_PLAYER] = PODVector<CheckerInfo*>(0);
    }

    DeckInfo(String id)
    {
        m_Id = id;
        m_DeckByPlayers[FIRST_PLAYER] = PODVector<CheckerInfo*>(0);
        m_DeckByPlayers[SECOND_PLAYER] = PODVector<CheckerInfo*>(0);
    }

    DeckInfo(const DeckInfo& deckInfo)
    {
        m_Id = deckInfo.m_Id;
        m_DeckByCheckerPos = deckInfo.m_DeckByCheckerPos;
        m_DeckByPlayers = deckInfo.m_DeckByPlayers;
    }

    inline const unsigned int GetId() const { return m_Id.ToHash(); }
    const DeckByCheckerPos& GetDeckByCheckerPos() const { return m_DeckByCheckerPos; }
    const DeckByPlayers& GetDeckByPlayers() const { return m_DeckByPlayers; }
    const PODVector<CheckerInfo*>& GetPlayerDeck(PlayerType playerType) { return m_DeckByPlayers[playerType]; }

    inline void SetId(const String& id) { m_Id = id; }
    bool AddChecker(CheckerInfo* checker);
    bool RemoveChecker(const CheckerPos& pos);
    bool MoveChecker(CheckerInfo* checker, CheckerPos newPos);

    bool IsContainChecker(CheckerInfo* checker);

private:
    String m_Id;
    DeckByCheckerPos m_DeckByCheckerPos;
    DeckByPlayers m_DeckByPlayers;
};

// Wrap DeckInfo class
class Deck
{

public:
    Deck()
    {
        m_Id = "Invalid";
    }

    Deck(const String& id)
    {
        m_Id = id;
    }

    Deck(const Deck& deck)
    {
        m_Id = deck.m_Id;
        m_pDeckInfo = deck.m_pDeckInfo;
    }

    inline const unsigned int GetId() const { return m_Id.ToHash(); }
    inline DeckInfo* GetDeckInfo() const { return m_pDeckInfo; }

    inline void SetId(const String& id) { m_Id = id; }
    inline void SetDeckInfo(DeckInfo* deckInfo) { m_pDeckInfo = deckInfo; }

private:
    String m_Id;
    DeckInfo* m_pDeckInfo;
};


class CheckerFactory 
{
	// Checker 
	typedef Vector<CheckerInfo> CheckersPool;
	typedef HashMap<unsigned int, CheckerInfo> CheckersInfoPool;
	typedef HashMap<unsigned int, CheckerInfo*> InactiveCheckers;
	typedef HashMap<unsigned int, CheckerInfo*> ActiveCheckers;

	// Deck
	typedef HashMap<unsigned int, DeckInfo> DecksPool;
	typedef HashMap<unsigned int, DeckInfo*> InactiveDecks;
	typedef HashMap<unsigned int, DeckInfo*> ActiveDecks;

public:
    CheckerFactory(CornersLogic* pCornersLogic);
   
	CheckerInfo* CreateChecker(const String& id, CheckerPos pos,
		int stepLeft, PlayerType playerType, Node* pNode);

	DeckInfo* CreateDeck(const String& id, const PODVector<CheckerInfo*>& checkers);
	DeckInfo* MoveCheckedInDeck(const String& id, DeckInfo* deck, const CheckerPos& checkerPos, const CheckerPos& newCheckerPos);
	DeckInfo* AppendDeck(DeckInfo* deck, const PODVector<CheckerInfo*>& checkers);

	unsigned int GetNextFreeDeckId() { return ++m_NextFreeDeckId; }
	unsigned int GetNextFreeCheckerId() { return ++m_NextFreeCheckerkId; }
	void ResetNextDeckId() { m_NextFreeDeckId = 0; }
	void ResetCheckerId() { m_NextFreeCheckerkId = 0; }
	void ResetPool();

protected:
	CheckerInfo* TakeCheckerFromPool(const String& id);
	bool PutCheckerInPool(CheckerInfo* checkerInfo);

	DeckInfo* TakeDeckFromPool(const String& id);
	bool PutDeckInPool(DeckInfo* deckInfo);

private:
	CornersLogic* m_pCornersGameLogic;

	// Checker pool
	CheckersPool m_CheckersPool;
	CheckersInfoPool m_CheckerPool;
	InactiveCheckers m_InactiveCheckers;
	ActiveCheckers m_ActiveCheckers;

	// Deck pool
	DecksPool m_DecksPool;
	InactiveDecks m_InactiveDecks;
	ActiveDecks m_ActiveDecks;

	const static unsigned int CHECKER_POOL_SIZE;
	const static unsigned int DECK_POOL_SIZE;

	unsigned int m_NextFreeCheckerkId;
	unsigned int m_NextFreeDeckId;

	unsigned int m_NextFreeCheckerFromPoolPos;
};
