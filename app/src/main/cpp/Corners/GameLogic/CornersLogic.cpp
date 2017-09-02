#include "CornersStd.h"

#include <GameEngine/Events/Client/ClientEvent.h>
#include <GameEngine/Events/Database/DatabaseEvent.h>

// Subscribe to events from checker controller
#include "Controllers/CheckerController.h"

#include "Factory/CheckerFactory.h"

// Game views
#include "GameView/MainMenuView.h"
#include "GameView/CornersHumanView.h"


CornersLogic::CornersLogic(Context* context) : BaseGameLogic(context)
{
	m_MinFieldSize = Vector2(1, 1);
	m_MaxFieldSize = Vector2(8, 8);
	m_PlayersChessCount = Vector2(9, 9);
	m_FirstPlayerHouse = Rect(1, 1, 3, 3);
	m_SecondPlayerHouse = Rect(6, 6, 8, 8);

	m_pCornersDatabase = static_cast<CornersDatabase*>(g_pApp->GetDatabase());

    m_bIsFirstPlayerInitialized = false;
    m_bIsGameFinished = false;
    m_bIsGameTryToFinish = false;

	m_CurrentPlayerStep = FIRST_PLAYER;

    m_pCurrentDeckInfo = nullptr;
    m_pEndDeckInfo = nullptr;

    m_SelectedCircleId = INVALID_NODE_ID;
}

bool CornersLogic::VInitialize()
{
     BaseGameLogic::VInitialize();

     m_pCheckerFactory = new CheckerFactory(this);

     g_pApp->GetGameOptions()->SetSceneName("Scenes/Checker.xml");

	 String path = g_pApp->GetFileSystem()->GetAppPreferencesDir("bachelor", "corners");
	 path.Append("corners.db");

	 DbConnection* dbConnection = g_pApp->ConnectToDB(path);
	 if(!dbConnection)
	 {
	    URHO3D_LOGERROR("Failed to connect to DB with path + " + path);
        g_pApp->AbortGame();
	 }
	 else
	 {
         m_pCornersDatabase->CreatePlayersTable();
		 // Create guest data
		 const PlayerData& guestData = m_pCornersDatabase->CreateGuestData();
		 // Get player data
		 const PlayerData& playerData = m_pCornersDatabase->GetPlayerDataIfExist(PLAYER_ID);
		 // If not exist yet, create initial player data with username "Player"
		 if(playerData.m_Username == "Invalid")
		 {
			 const PlayerData& newPlayerData = m_pCornersDatabase->CreatePlayerData("Player", 0, 0);
		 }
	 }

    SubscribeToEvent(LOCAL_EVENT_MOUSE_CHECKER_SELECTED, URHO3D_HANDLER(CornersLogic, CheckerSelectedDelegate));
    SubscribeToEvent(LOCAL_EVENT_MOUSE_CIRCLE_SELECTED, URHO3D_HANDLER(CornersLogic, CircleSelectedDelegate));
    SubscribeToEvent(EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME, URHO3D_HANDLER(CornersLogic, TryPlayAgainGameDelegate));

    return true;
}

void CornersLogic::VShutdown()
{
    BaseGameLogic::VShutdown();

	SAFE_DELETE(m_pCheckerFactory);
}

void CornersLogic::VOnUpdate(float timeStep)
{
    BaseGameLogic::VOnUpdate(timeStep);

    switch (m_State)
    {
		case BGS_MainMenu:
		{

			break;
		}

		case BGS_WaitingForPlayers: // this case runs when pressed "Start" button
		{
			break;
		}

		case BGS_Running: 
		{
			if (!m_bIsGameFinished && !m_bIsGameTryToFinish)
			{
                // After first player turn, second player must do the last turn
				if (IsGameFinish(FIRST_PLAYER) != INVALID_PLAYER)
                {
                    m_bIsGameTryToFinish = true;
                }
                //
				else if (IsGameFinish(SECOND_PLAYER) != INVALID_PLAYER)
				{
                    m_bIsGameFinished = true;

					const PlayerData& secondPlayerData = GetSecondPlayer();
					m_pCornersDatabase->UpdateStatistics(GUEST_PLAYER_ID, secondPlayerData.m_Wins + 1, secondPlayerData.m_Loses);

					const PlayerData& firstPlayerData = GetFirstPlayer();
					m_pCornersDatabase->UpdateStatistics(PLAYER_ID, firstPlayerData.m_Wins, firstPlayerData.m_Loses + 1);

					VariantMap data;
					data[CLIENT_FINISH_GAME::P_WINNER_PLAYER_NAME] = secondPlayerData.m_Username;
					data[CLIENT_FINISH_GAME::P_PLAYER_TYPE] = PlayerType::SECOND_PLAYER;    // who win ?
					SendEvent(EVENT_DATA_CLIENT_FINISH_GAME, data);

                    URHO3D_LOGERROR(String("Game is end! Second player is Won !"));
				}
			}
			else if(m_bIsGameTryToFinish && m_CurrentPlayerStep == FIRST_PLAYER)
			{
                // If second player also finish game, then DRAW
				if(IsGameFinish(SECOND_PLAYER) != INVALID_PLAYER)
				{
					m_bIsGameTryToFinish = false;
					m_bIsGameFinished = true;

					VariantMap data;
					data[CLIENT_FINISH_GAME::P_WINNER_PLAYER_NAME] = "Draw";
					data[CLIENT_FINISH_GAME::P_PLAYER_TYPE] = PlayerType::INVALID_PLAYER;
					SendEvent(EVENT_DATA_CLIENT_FINISH_GAME, data);
					URHO3D_LOGERROR(String("Game is end! Draw !"));
				}
				else
				{
					m_bIsGameTryToFinish = false;
					m_bIsGameFinished = true;

					const PlayerData& firstPlayerData = GetFirstPlayer();
					m_pCornersDatabase->UpdateStatistics(PLAYER_ID, firstPlayerData.m_Wins + 1, firstPlayerData.m_Loses);

					const PlayerData& secondPlayerData = GetSecondPlayer();
					m_pCornersDatabase->UpdateStatistics(GUEST_PLAYER_ID, secondPlayerData.m_Wins, secondPlayerData.m_Loses + 1);

					VariantMap data;
					data[CLIENT_FINISH_GAME::P_WINNER_PLAYER_NAME] = firstPlayerData.m_Username;
					data[CLIENT_FINISH_GAME::P_PLAYER_TYPE] = PlayerType::FIRST_PLAYER;
					SendEvent(EVENT_DATA_CLIENT_FINISH_GAME, data);

					URHO3D_LOGERROR(String("Game is end! First player is Won !"));
				}
			}
			break;
		}
    }
}

void CornersLogic::VChangeState(enum BaseGameState newState)
{
    BaseGameLogic::VChangeState(newState);

    if (newState == BGS_MainMenu)
    {
		
    }
    else if (newState == BGS_WaitingForPlayers)
    {
        SharedPtr<HumanView> mainMenuView = StaticCast<HumanView>(m_GameViews.Front());
        mainMenuView->VOnRestore(); // hide main menu UI

        // spawn all local players (should only be one, though we might support more in the future)
        assert(m_ExpectedPlayers == 1);
        for (int i = 0; i < m_ExpectedPlayers; i++)
        {
            SharedPtr<IGameView> cornersGameView(new CornersHumanView(context_, g_pApp->GetRenderer()));
            VAddView(cornersGameView);
        }
    }
    else if (newState == BGS_SpawningPlayersActors)
    {

    }
}

bool CornersLogic::VLoadGameDelegate(String pLevelData)
{
    if (!m_bIsFirstPlayerInitialized)
    {
        if (!InitEnvironment())
        {
            g_pApp->AbortGame();
        }

        if (!InitPlayer(FIRST_PLAYER))
        {
            g_pApp->AbortGame();
        }


        if (!InitPlayer(SECOND_PLAYER))
        {
            g_pApp->AbortGame();
        }

      // SetTestPositions();
    }
    return true;
}

void CornersLogic::SetTestPositions()
{
    const PODVector<CheckerInfo*>& currentFirstPlayerDeck = m_pCurrentDeckInfo->GetPlayerDeck(FIRST_PLAYER);
    if (!currentFirstPlayerDeck.Empty())
    {
        const PODVector<CheckerInfo*>& endPlayerDeck = m_pEndDeckInfo->GetPlayerDeck(FIRST_PLAYER);

        const DeckByPlayers& deckByPlayers = m_pCurrentDeckInfo->GetDeckByPlayers();
        PODVector<CheckerInfo*>* firstPlayerDeck = deckByPlayers[FIRST_PLAYER];

        int i = 0;
        for (auto it = firstPlayerDeck->Begin(); it != firstPlayerDeck->End(); it++)
        {
            CheckerInfo* checker = (*it);

            CheckerPos currentPos = checker->GetCheckerPos();
            CheckerPos newCheckerPos = endPlayerDeck[i]->GetCheckerPos();
            Matrix4 newCheckerPos3D = CheckerPos::CalculateCheckerPos3D(newCheckerPos);

            if (checker)
            {
                if (i == 8)
                {
                    newCheckerPos = CheckerPos(Vector2(5,5));
                    newCheckerPos3D = CheckerPos::CalculateCheckerPos3D(newCheckerPos);

                    checker->SetCheckerPos(newCheckerPos);
                    checker->GetNode()->SetPosition(newCheckerPos3D.Translation());

                    m_pCurrentDeckInfo->RemoveChecker(currentPos);
                }
                else
                {
                    checker->SetCheckerPos(newCheckerPos);
                    checker->GetNode()->SetPosition(newCheckerPos3D.Translation());
                }
            }

            if (!m_pCurrentDeckInfo->MoveChecker(checker, newCheckerPos))
            {
                URHO3D_LOGERROR("Failed to move checked in deck");
                g_pApp->AbortGame();
            }

            i++;
        }
    }

    const PODVector<CheckerInfo*>& currentSecondPlayerDeck = m_pCurrentDeckInfo->GetPlayerDeck(SECOND_PLAYER);
    if (!currentSecondPlayerDeck.Empty())
    {
        const PODVector<CheckerInfo*>& endPlayerDeck = m_pEndDeckInfo->GetPlayerDeck(
                SECOND_PLAYER);

        const DeckByPlayers& deckByPlayers = m_pCurrentDeckInfo->GetDeckByPlayers();
        PODVector<CheckerInfo*>* secondPlayerDeck = deckByPlayers[SECOND_PLAYER];

        int i = 0;
        for (auto it = secondPlayerDeck->Begin(); it != secondPlayerDeck->End(); it++)
        {
            CheckerInfo* checker = (*it);

            CheckerPos currentPos = checker->GetCheckerPos();
            CheckerPos newCheckerPos = endPlayerDeck[i]->GetCheckerPos();
            Matrix4 newCheckerPos3D = CheckerPos::CalculateCheckerPos3D(newCheckerPos);

            if (checker)
            {
                if (i == 8)
                {
                    newCheckerPos = CheckerPos(Vector2(4,4));
                    newCheckerPos3D = CheckerPos::CalculateCheckerPos3D(newCheckerPos);

                    checker->SetCheckerPos(newCheckerPos);
                    checker->GetNode()->SetPosition(newCheckerPos3D.Translation());

                    m_pCurrentDeckInfo->RemoveChecker(currentPos);
                }
                else
                {
                    checker->SetCheckerPos(newCheckerPos);
                    checker->GetNode()->SetPosition(newCheckerPos3D.Translation());
                }
            }

            if (!m_pCurrentDeckInfo->MoveChecker(checker, newCheckerPos))
            {
                URHO3D_LOGERROR("Failed to move checked in deck");
                g_pApp->AbortGame();
            }

            i++;
        }
    }
}

bool CornersLogic::InitEnvironment()
{
    XMLFile* prefabFile = g_pApp->GetResCache()->GetResource<XMLFile>("Prefabs/CircleSelectGreen.xml");
    Node* firstCircleSelectParent = m_pScene->GetChild("FirstPlayerCircleSelects");
    if (!firstCircleSelectParent)
    {
        URHO3D_LOGERROR("Failed to find firstCircleSelectParent node");
        return false;
    }

    Vector2 currentPos = Vector2(1, 1);
    int deckArea = m_MaxFieldSize.x_ * m_MaxFieldSize.y_;
    for (int i = 0; i < deckArea; i++)
    {
        CheckerPos checkerPos(currentPos);
        const Matrix4 checkerPos3D = CheckerPos::CalculateCheckerPos3D(checkerPos);

        Node* pCircleSelectGreen = firstCircleSelectParent->CreateChild("CircleSelectGreen" + checkerPos.GetPos().ToString(), CreateMode::LOCAL);

        if(prefabFile)
        {
            if (pCircleSelectGreen->LoadXML(prefabFile->GetRoot()))
            {
                if(pCircleSelectGreen)
                {
                    pCircleSelectGreen->SetPosition(checkerPos3D.Translation());
                    pCircleSelectGreen->SetVar("CheckerPos", checkerPos.GetPos());
                    pCircleSelectGreen->SetEnabled(false);
                    pCircleSelectGreen->SetVar("Player", PlayerType::FIRST_PLAYER);
                    m_FirstCircleSelects[currentPos.ToString()] = pCircleSelectGreen;
                }
            }
            else
            {
                URHO3D_LOGERROR("Failed to create Green Circle Select !");
            }
        }

        if (currentPos.x_ >= m_MaxFieldSize.x_)
        {
            currentPos.x_ = 1;
            currentPos.y_ += 1;
        }
        else
        {
            currentPos.x_ += 1;
        }
    }

    prefabFile = g_pApp->GetResCache()->GetResource<XMLFile>("Prefabs/CircleSelectRed.xml");
    Node* secondCircleSelectParent = m_pScene->GetChild("SecondPlayerCircleSelects");
    if (!secondCircleSelectParent)
    {
        URHO3D_LOGERROR("Failed to find secondCircleSelectParent node !");
        return false;
    }

    currentPos = Vector2(1, 1);
    for (int i = 0; i < deckArea; i++)
    {
        CheckerPos checkerPos(currentPos);
        const Matrix4 checkerPos3D = CheckerPos::CalculateCheckerPos3D(checkerPos);

        Node* pCircleSelectRed = secondCircleSelectParent->CreateChild("CircleSelectRed" + checkerPos.GetPos().ToString(), CreateMode::LOCAL);
        if(prefabFile)
        {
            if (pCircleSelectRed->LoadXML(prefabFile->GetRoot()))
            {
                if(pCircleSelectRed)
                {
                    pCircleSelectRed->SetPosition(checkerPos3D.Translation());
                    pCircleSelectRed->SetVar("CheckerPos", checkerPos.GetPos());
                    pCircleSelectRed->SetEnabled(false);
                    pCircleSelectRed->SetVar("Player", PlayerType::SECOND_PLAYER);
                    m_SecondCircleSelects[currentPos.ToString()] = pCircleSelectRed;
                }
            }
            else
            {
                URHO3D_LOGERROR("Failed to create Green Circle Select !");
            }
        }

        if (currentPos.x_ >= m_MaxFieldSize.x_)
        {
            currentPos.x_ = 1;
            currentPos.y_ += 1;
        }
        else
        {
            currentPos.x_ += 1;
        }
    }

    return true;
}

bool CornersLogic::InitPlayer(PlayerType playerType)
{
	PODVector<CheckerInfo*> checkers;
	checkers.Reserve((unsigned int)(playerType == FIRST_PLAYER ? m_PlayersChessCount.x_ : m_PlayersChessCount.y_));

	Vector2 currentPos = Vector2::ZERO;
	Vector2 endPos = Vector2::ZERO;
	if (playerType == FIRST_PLAYER)
	{
		currentPos = m_FirstPlayerHouse.min_;
		endPos = m_FirstPlayerHouse.max_;
	}
	else
	{
		currentPos = m_SecondPlayerHouse.min_;
		endPos = m_SecondPlayerHouse.max_;
	}
	
	int startPosX = (int)currentPos.x_;

	for (int i = 0; i < m_PlayersChessCount.x_; i++)
	{
		CheckerPos newCheckerPos(currentPos);

		unsigned int nextId = m_pCheckerFactory->GetNextFreeCheckerId();

		CheckerInfo* pNewChecker = CreateChecker(playerType == FIRST_PLAYER ? "Prefabs/GreenChecker.xml" : "Prefabs/RedChecker.xml",
                                                 nullptr,
												 playerType == FIRST_PLAYER ? String("1_Player_" + String(nextId)) : String("2_Player_" + String(nextId)),
												 newCheckerPos,
											     playerType,
												 40);

		if (pNewChecker)
		{
			checkers.Push(pNewChecker);
		}
		else
		{
			return false;
		}

		if (currentPos.x_ >= endPos.x_)
		{
			currentPos.y_ += 1.0f;
			currentPos.x_ = (float)startPosX;
		}
		else
		{
			currentPos.x_ += 1;
		}
	}

	if (playerType == SECOND_PLAYER)
	{
		m_pCheckerFactory->AppendDeck(m_pCurrentDeckInfo, checkers);
        m_bIsSecondPlayerInitialized  = true;
	}
	else // FIRST_PLAYER
	{
		unsigned int nextId = m_pCheckerFactory->GetNextFreeDeckId();
        m_pCurrentDeckInfo = m_pCheckerFactory->CreateDeck(String("Deck_") + String(nextId), checkers);
		if (nextId >= Urho3D::M_MAX_UNSIGNED)
		{
			m_pCheckerFactory->ResetNextDeckId();
			URHO3D_LOGERROR("Next deck id reached uint bound. (== Urho3D::M_MAX_UNSIGNED");
		}
        m_bIsFirstPlayerInitialized = true;
	}

	if (!m_pCurrentDeckInfo)
	{
		ErrorDialog("First player initialization", "Error: Game player initialization failed. m_pCurrentDeckInfo == NULL.");
		g_pApp->AbortGame();
		return false;
	}
    else
    {
        m_pEndDeckInfo = CreateEndDeck(playerType);
        if (!m_pEndDeckInfo)
        {
            ErrorDialog("First player initialization", "Error: Game player initialization failed. m_pEndDeckInfo == NULL.");
            return false;
        }
    }

	return true;
}


CheckerInfo* CornersLogic::CreateChecker(String prefabPath, Node* parent, const String& id, const CheckerPos& pos, PlayerType playerType, int leftStep)
{
    Node* pChecker = nullptr;
    XMLFile* prefabFile = g_pApp->GetResCache()->GetResource<XMLFile>(prefabPath);
    if(prefabFile)
    {
        if(parent)
        {
            pChecker = parent->CreateChild(id, CreateMode::LOCAL);
        }
        else
        {
            pChecker = m_pScene->CreateChild(id, CreateMode::LOCAL);
        }

        if (pChecker->LoadXML(prefabFile->GetRoot()))
        {
            const Matrix4 checkerPos3D = CheckerPos::CalculateCheckerPos3D(pos);

            pChecker->SetPosition(checkerPos3D.Translation());
            pChecker->AddTag("Checker");
            pChecker->SetVar("Player", playerType);
        }
        else
        {
            ErrorDialog("First player initialization", "Error: Game first player initialization failed. LoadXML returns false.");
            return NULL;
        }
    }
    else
    {
        ErrorDialog("First player initialization", "Error: Game first player initialization failed. prefabFile == NULL.");
        return NULL;
    }

    CheckerInfo* pNewChecker = m_pCheckerFactory->CreateChecker(id, pos, leftStep, playerType, pChecker);
    if (pNewChecker)
    {
        return pNewChecker;
    }
    else
    {
        ErrorDialog("First player initialization", "Error: Game first player initialization failed. pNewChecker == NULL.");
        return NULL;
    }

    return NULL;
}

DeckInfo* CornersLogic::CreateEndDeck(PlayerType playerType)
{
    if (m_pCurrentDeckInfo)
    {
        PODVector<CheckerInfo*> checkers;
        checkers.Reserve((unsigned int)(playerType == FIRST_PLAYER ? m_PlayersChessCount.x_ : m_PlayersChessCount.y_));

        Vector2 currentPos = Vector2::ZERO;
        Vector2 endPos = Vector2::ZERO;
        if (playerType == FIRST_PLAYER)
        {
            currentPos = m_SecondPlayerHouse.min_;
            endPos = m_SecondPlayerHouse.max_;
        }
        else
        {
            currentPos = m_FirstPlayerHouse.min_;
            endPos = m_FirstPlayerHouse.max_;
        }

        int startPosX = (int)currentPos.x_;

        //const DeckByCheckerPos& deck = pDeckInfo->GetDeck();

        const PODVector<CheckerInfo*>& deck = m_pCurrentDeckInfo->GetPlayerDeck(playerType);
        int i = 0;
        for (PODVector<CheckerInfo*>::ConstIterator it = deck.Begin(); it != deck.End(); it++)
        {
            CheckerPos newCheckerPos(currentPos);
            Node* pChecker = (*it)->GetNode();

            unsigned int nextId = m_pCheckerFactory->GetNextFreeCheckerId();
            assert(nextId <= Urho3D::M_MAX_UNSIGNED);

            String newId = playerType == FIRST_PLAYER ? String("1_Player_" + String(nextId)) : String("2_Player_" + String(nextId));
            CheckerInfo* pNewChecker = m_pCheckerFactory->CreateChecker(newId, newCheckerPos, 40, playerType, pChecker);
            if (!pNewChecker)
            {
                ErrorDialog("GG end node initialization", "Error: Game graph node initialization failed. pNewChecker == NULL.");
                return NULL;
            }
            else
            {
                checkers.Push(pNewChecker);
            }

            if (currentPos.x_ >= endPos.x_)
            {
                currentPos.y_ += 1;
                currentPos.x_ = (float)startPosX;
            }
            else
            {
                currentPos.x_ += 1;
            }

            i++;
        }

        if(playerType == SECOND_PLAYER)
        {
            m_pCheckerFactory->AppendDeck(m_pEndDeckInfo, checkers);
        }
        else
        {
            // First player create start initialize end GG node
            unsigned int nextId = m_pCheckerFactory->GetNextFreeDeckId();
            m_pEndDeckInfo = m_pCheckerFactory->CreateDeck(String("EndDeck_") + String(nextId), checkers);
            if (nextId >= Urho3D::M_MAX_UNSIGNED)
            {
                m_pCheckerFactory->ResetNextDeckId();
                URHO3D_LOGERROR("Next deck id reached uint bound. (== Urho3D::M_MAX_UNSIGNED");
            }
        }

        if (!m_pEndDeckInfo)
        {
            ErrorDialog("GG end node initialization", "Error: DeckInfo node initialization failed. pNewDeckInfo == NULL.");
            return NULL;
        }

        return m_pEndDeckInfo;
    }

    return NULL;
}


PlayerType CornersLogic::IsGameFinish(const PlayerType& playerType)
{
	PlayerType result = INVALID_PLAYER;

	const PODVector<CheckerInfo*>& currentFirstPlayerDeck = m_pCurrentDeckInfo->GetPlayerDeck(playerType);
	if (!currentFirstPlayerDeck.Empty())
	{
		const PODVector<CheckerInfo*>& endPlayerDeck = m_pEndDeckInfo->GetPlayerDeck(playerType);

		const DeckByCheckerPos& deckByPos = m_pCurrentDeckInfo->GetDeckByCheckerPos();

		bool isFound = true;
		for (int i = 0; i != endPlayerDeck.Size(); i++)
		{
			CheckerInfo* checker = NULL;
			deckByPos.TryGetValue(endPlayerDeck[i]->GetCheckerPos().GetPos().ToString(), checker);

			if (checker)
			{
				PlayerType checkerPlayerType = checker->GetPlayerType();
				if (checkerPlayerType != playerType)
				{
					result = INVALID_PLAYER;
					isFound = false;
					break;
				}
			}
			else
			{
				result = INVALID_PLAYER;
				isFound = false;
				break;
			}
		}

		if (isFound)
		{
			result = playerType;
		}
	}
	
	return result;
}

void CornersLogic::ResetGame()
{
	m_pCheckerFactory->ResetPool();
	m_CurrentPlayerStep = FIRST_PLAYER;

	if (m_pScene)
	{
		PODVector<Node*> checkers;
        m_pScene->GetChildrenWithTag(checkers, "Checker");
		if (checkers.Size() != 0)
		{
			if (!checkers.Empty())
			{
				for (int i = 0; i < checkers.Size(); i++)
				{
                    m_pScene->RemoveChild(checkers[i]);
				}
			}
		}
	}
	else
	{
		URHO3D_LOGERROR("Failed to find LocalChecker scene");
		g_pApp->AbortGame();
	}

	if (!InitPlayer(FIRST_PLAYER))
	{
		g_pApp->AbortGame();
	}


	if (!InitPlayer(SECOND_PLAYER))
	{
		g_pApp->AbortGame();
	}

    if (!m_ActiveCircleSelects.Empty())
    {
        for (HashMap<String, Node*>::Iterator it = m_FirstCircleSelects.Begin(); it != m_FirstCircleSelects.End(); it++)
        {
            (*it).second_->SetEnabled(false);
        }

        for (HashMap<String, Node*>::Iterator it = m_SecondCircleSelects.Begin(); it != m_SecondCircleSelects.End(); it++)
        {
            (*it).second_->SetEnabled(false);
        }

        m_ActiveCircleSelects.Clear();
    }

	m_bIsGameTryToFinish = false;
	m_bIsGameFinished = false;
}

bool CornersLogic::IsNotInDeck(const Vector2 & pos)
{
	return pos.x_ < m_MinFieldSize.x_ || pos.y_ < m_MinFieldSize.y_ ||
		pos.x_ > m_MaxFieldSize.x_ || pos.y_ > m_MaxFieldSize.y_;
}

void CornersLogic::CheckerSelectedDelegate(StringHash eventType, VariantMap & eventData)
{
	Scene* pScene = (Scene*)eventData[MOUSE_CHECKER_SELECTED::P_SCENE].GetPtr();
	NodeId id = eventData[MOUSE_CHECKER_SELECTED::P_NODE_ID].GetUInt();

	if (id != INVALID_NODE_ID)
	{
		Node* pChecker = pScene->GetNode(id);

		if (id != m_SelectedCircleId)
		{
            m_SelectedCircleId = id;
			PlayerType playerType = (PlayerType)pChecker->GetVar("Player").GetInt();
			
			if (!m_ActiveCircleSelects.Empty())
			{
				for (HashMap<String, Node*>::Iterator it = m_FirstCircleSelects.Begin(); it != m_FirstCircleSelects.End(); it++)
				{
					(*it).second_->SetEnabled(false);
				}

				for (HashMap<String, Node*>::Iterator it = m_SecondCircleSelects.Begin(); it != m_SecondCircleSelects.End(); it++)
				{
					(*it).second_->SetEnabled(false);
				}

				m_ActiveCircleSelects.Clear();
			}

			const CheckerPos checkerPos = CheckerPos::CalculateCheckedPosFrom3D(pChecker->GetPosition());

			PODVector<Vector2> allPossibleTurns;
			PODVector<Vector2> ecxepts;

			GetAllPossibleTurns(m_pCurrentDeckInfo, checkerPos, ecxepts, &allPossibleTurns, true);

			
			if (playerType == PlayerType::FIRST_PLAYER)
			{
				if (!allPossibleTurns.Empty())
				{
          					for (int i = 0; i < allPossibleTurns.Size(); i++)
					{
						const Vector2& pos = allPossibleTurns[i];
						m_FirstCircleSelects[pos.ToString()]->SetEnabled(true);
						m_FirstCircleSelects[pos.ToString()]->SetVar("SelectedCheckerPos", checkerPos.GetPos());
						m_ActiveCircleSelects.Push(pos);
					}
				}
			}
			else if (playerType == PlayerType::SECOND_PLAYER)
			{
				if (!allPossibleTurns.Empty())
				{
					for (int i = 0; i < allPossibleTurns.Size(); i++)
					{
						const Vector2& pos = allPossibleTurns[i];
						m_SecondCircleSelects[pos.ToString()]->SetEnabled(true);
						m_SecondCircleSelects[pos.ToString()]->SetVar("SelectedCheckerPos", checkerPos.GetPos());
						m_ActiveCircleSelects.Push(pos);
					}
				}
			}
		}
	}
}

void CornersLogic::CircleSelectedDelegate(StringHash eventType, VariantMap & eventData)
{
	Scene* pScene = (Scene*)eventData[MOUSE_CHECKER_SELECTED::P_SCENE].GetPtr();
	NodeId id = eventData[MOUSE_CHECKER_SELECTED::P_NODE_ID].GetUInt();

	if (id != INVALID_NODE_ID)
	{
		Node* pCircle = pScene->GetNode(id);
		if (id != m_SelectedCircleId)
		{
            m_SelectedCircleId = id;


			for (HashMap<String, Node*>::Iterator it = m_FirstCircleSelects.Begin(); it != m_FirstCircleSelects.End(); it++)
			{
				(*it).second_->SetEnabled(false);
			}
				
			for (HashMap<String, Node*>::Iterator it = m_SecondCircleSelects.Begin(); it != m_SecondCircleSelects.End(); it++)
			{
				(*it).second_->SetEnabled(false);
			}
			

			m_ActiveCircleSelects.Clear();
		
			CheckerPos checkerPos(pCircle->GetVar("SelectedCheckerPos").GetVector2());
			const CheckerPos newCheckerPos(pCircle->GetVar("CheckerPos").GetVector2());

			CheckerInfo* selectedChecker = NULL;
			const DeckByCheckerPos& deck = m_pCurrentDeckInfo->GetDeckByCheckerPos();
            deck.TryGetValue(checkerPos.GetPos().ToString(), selectedChecker);
			if (selectedChecker)
			{
				selectedChecker->SetCheckerPos(newCheckerPos);
				selectedChecker->SetLeftStep(selectedChecker->GetLeftStep() - 1);

				const Matrix4 checkerPos3D = CheckerPos::CalculateCheckerPos3D(newCheckerPos);
				selectedChecker->GetNode()->SetPosition(checkerPos3D.Translation());

               // m_pCurrentDeckInfo->AddChecker(selectedChecker);
				//if (!m_pCurrentDeckInfo->RemoveChecker(checkerPos))
				//{
				//	URHO3D_LOGERROR("Failed to remove checked from deck. CircleSelectedDelegate");
				//	g_pApp->AbortGame();
				//}
                if (!m_pCurrentDeckInfo->MoveChecker(selectedChecker, newCheckerPos))
                {
                    URHO3D_LOGERROR("Failed to move checked in deck");
                    g_pApp->AbortGame();
                }

                if (!m_pCurrentDeckInfo->RemoveChecker(checkerPos))
                {
                	URHO3D_LOGERROR("Failed to remove checked from deck. CircleSelectedDelegate");
                	g_pApp->AbortGame();
                }

                m_CurrentPlayerStep = m_CurrentPlayerStep == FIRST_PLAYER ? SECOND_PLAYER : FIRST_PLAYER;

				VariantMap data;
				data[CLIENT_FINISH_TURN::P_PLAYER_TYPE] = m_CurrentPlayerStep;
				data[CLIENT_FINISH_TURN::P_OLD_POSITION] = Vector2(checkerPos.GetPos());
				data[CLIENT_FINISH_TURN::P_NEW_POSITION] = Vector2(newCheckerPos.GetPos());
				SendEvent(EVENT_DATA_CLIENT_FINISH_TURN, data);
			}
			else
			{
				URHO3D_LOGERROR("Failed to find selectedChecker. CircleSelectedDelegate");
				g_pApp->AbortGame();
			}
		}
	}
}

void CornersLogic::TryPlayAgainGameDelegate(StringHash eventType, VariantMap& eventData)
{
	ResetGame();
}



void CornersLogic::GetAllPossibleTurns(DeckInfo* deck, const CheckerPos & checkerPos, PODVector<Vector2>& excepts, PODVector<Vector2>* result, bool isInitialPosition)
{
    if (!result)
    {
        return;
    }

    const Vector2& pos = checkerPos.GetPos();
    CheckerMoveType newMove = CheckerMoveType::INVALID;

    // Put at once current pos in excepts
    if(isInitialPosition)
    {
        excepts.Push(pos);
    }

    Vector2 m_TopCheckerPos;
    Vector2 m_BottomCheckerPos;
    Vector2 m_LeftCheckerPos;
    Vector2 m_RightCheckerPos;

    CheckerInfo* m_pTopCheckerInfo = NULL;
    CheckerInfo* m_pBottomCheckerInfo = NULL;
    CheckerInfo* m_pLeftCheckerInfo = NULL;
    CheckerInfo* m_pRightCheckerInfo = NULL;

    Vector2 m_NextTopCheckerPos;
    Vector2 m_NextBottomCheckerPos;
    Vector2 m_NextLeftCheckerPos;
    Vector2 m_NextRightCheckerPos;

    CheckerInfo* m_pNextTopCheckerInfo = NULL;
    CheckerInfo* m_pNextBottomCheckerInfo = NULL;
    CheckerInfo* m_pNextLeftCheckerInfo = NULL;
    CheckerInfo* m_pNextRightCheckerInfo = NULL;

    m_TopCheckerPos = pos + Vector2(0, +1);
    m_BottomCheckerPos = pos + Vector2(0, -1);
    m_LeftCheckerPos = pos + Vector2(-1, 0);
    m_RightCheckerPos = pos + Vector2(+1, 0);

    m_TopCheckerPos = IsNotInDeck(m_TopCheckerPos) ? Vector2::ZERO : m_TopCheckerPos;
    m_BottomCheckerPos = IsNotInDeck(m_BottomCheckerPos) ? Vector2::ZERO : m_BottomCheckerPos;
    m_LeftCheckerPos = IsNotInDeck(m_LeftCheckerPos) ? Vector2::ZERO : m_LeftCheckerPos;
    m_RightCheckerPos = IsNotInDeck(m_RightCheckerPos) ? Vector2::ZERO : m_RightCheckerPos;

    if (!excepts.Empty())
    {
        for (int i = 0; i < excepts.Size(); i++)
        {
            if (excepts[i] == m_TopCheckerPos)
            {
                m_TopCheckerPos = Vector2::ZERO;
            }
            else if (excepts[i] == m_BottomCheckerPos)
            {
                m_BottomCheckerPos = Vector2::ZERO;
            }
            else if (excepts[i] == m_LeftCheckerPos)
            {
                m_LeftCheckerPos = Vector2::ZERO;
            }
            else if (excepts[i] == m_RightCheckerPos)
            {
                m_RightCheckerPos = Vector2::ZERO;
            }
        }
    }

    m_pTopCheckerInfo = NULL;
    m_pBottomCheckerInfo = NULL;
    m_pLeftCheckerInfo = NULL;
    m_pRightCheckerInfo = NULL;

    m_NextTopCheckerPos = m_TopCheckerPos + Vector2(0, +1);
    m_NextBottomCheckerPos = m_BottomCheckerPos + Vector2(0, -1);
    m_NextLeftCheckerPos = m_LeftCheckerPos + Vector2(-1, 0);
    m_NextRightCheckerPos = m_RightCheckerPos + Vector2(+1, 0);

    m_NextTopCheckerPos = IsNotInDeck(m_NextTopCheckerPos) ? Vector2::ZERO : m_NextTopCheckerPos;
    m_NextBottomCheckerPos = IsNotInDeck(m_NextBottomCheckerPos) ? Vector2::ZERO : m_NextBottomCheckerPos;
    m_NextLeftCheckerPos = IsNotInDeck(m_NextLeftCheckerPos) ? Vector2::ZERO : m_NextLeftCheckerPos;
    m_NextRightCheckerPos = IsNotInDeck(m_NextRightCheckerPos) ? Vector2::ZERO : m_NextRightCheckerPos;

    if (!excepts.Empty())
    {
        for (int i = 0; i < excepts.Size(); i++)
        {
            if (excepts[i] == m_NextTopCheckerPos)
            {
                m_NextTopCheckerPos = Vector2::ZERO;
            }
            else if (excepts[i] == m_NextBottomCheckerPos)
            {
                m_NextBottomCheckerPos = Vector2::ZERO;
            }
            else if (excepts[i] == m_NextLeftCheckerPos)
            {
                m_NextLeftCheckerPos = Vector2::ZERO;
            }
            else if (excepts[i] == m_NextRightCheckerPos)
            {
                m_NextRightCheckerPos = Vector2::ZERO;
            }
        }
    }

    m_pNextTopCheckerInfo = NULL;
    m_pNextBottomCheckerInfo = NULL;
    m_pNextLeftCheckerInfo = NULL;
    m_pNextRightCheckerInfo = NULL;

    const DeckByCheckerPos& deckByCheckerPos = deck->GetDeckByCheckerPos();

    if (m_TopCheckerPos != Vector2::ZERO)
    {
        deckByCheckerPos.TryGetValue(m_TopCheckerPos.ToString(), m_pTopCheckerInfo);
        if (m_pTopCheckerInfo)
        {
            if (m_NextTopCheckerPos != Vector2::ZERO)
            {
                deckByCheckerPos.TryGetValue(m_NextTopCheckerPos.ToString(), m_pNextTopCheckerInfo);
                if (!m_pNextTopCheckerInfo)
                {
                    result->Push(m_NextTopCheckerPos);
                    excepts.Push(m_NextTopCheckerPos);
                    excepts.Push(m_TopCheckerPos);

                    CheckerPos newMove(m_NextTopCheckerPos);
                    GetAllPossibleTurns(deck, newMove, excepts, result, false);
                }
            }
        }
        else
        {
            // If we do not step into recursion, then it is new possible move position
            // From start checker position
            if(isInitialPosition)
            {
                result->Push(m_TopCheckerPos);
                excepts.Push(m_TopCheckerPos);
            }
        }
    }

    if (m_BottomCheckerPos != Vector2::ZERO)
    {
        deckByCheckerPos.TryGetValue(m_BottomCheckerPos.ToString(), m_pBottomCheckerInfo);
        if (m_pBottomCheckerInfo)
        {
            if (m_NextBottomCheckerPos != Vector2::ZERO)
            {
                deckByCheckerPos.TryGetValue(m_NextBottomCheckerPos.ToString(), m_pNextBottomCheckerInfo);
                if (!m_pNextBottomCheckerInfo)
                {

                    result->Push(m_NextBottomCheckerPos);
                    excepts.Push(m_NextBottomCheckerPos);
                    excepts.Push(m_BottomCheckerPos);

                    CheckerPos newMove(m_NextBottomCheckerPos);
                    GetAllPossibleTurns(deck, newMove, excepts, result, false);
                }
            }
        }
        else
        {
            // If we do not step into recursion, then it is new possible move position
            // From start checker position
            if(isInitialPosition)
            {
                result->Push(m_BottomCheckerPos);
                excepts.Push(m_BottomCheckerPos);
            }
        }
    }

    if (m_LeftCheckerPos != Vector2::ZERO)
    {
        deckByCheckerPos.TryGetValue(m_LeftCheckerPos.ToString(), m_pLeftCheckerInfo);
        if (m_pLeftCheckerInfo)
        {
            if (m_NextLeftCheckerPos != Vector2::ZERO)
            {
                deckByCheckerPos.TryGetValue(m_NextLeftCheckerPos.ToString(), m_pNextLeftCheckerInfo);
                if (!m_pNextLeftCheckerInfo)
                {
                    result->Push(m_NextLeftCheckerPos);
                    excepts.Push(m_NextLeftCheckerPos);
                    excepts.Push(m_LeftCheckerPos);

                    CheckerPos newMove(m_NextLeftCheckerPos);
                    GetAllPossibleTurns(deck, newMove, excepts, result, false);
                }
            }
        }
        else
        {
            // If we do not step into recursion, then it is new possible move position
            // From start checker position
            if(isInitialPosition)
            {
                result->Push(m_LeftCheckerPos);
                excepts.Push(m_LeftCheckerPos);
            }
        }
    }

    if (m_RightCheckerPos != Vector2::ZERO)
    {
        deckByCheckerPos.TryGetValue(m_RightCheckerPos.ToString(), m_pRightCheckerInfo);
        if (m_pRightCheckerInfo)
        {
            if (m_NextRightCheckerPos != Vector2::ZERO)
            {
                deckByCheckerPos.TryGetValue(m_NextRightCheckerPos.ToString(), m_pNextRightCheckerInfo);
                if (!m_pNextRightCheckerInfo)
                {
                    result->Push(m_NextRightCheckerPos);
                    excepts.Push(m_NextRightCheckerPos);
                    excepts.Push(m_RightCheckerPos);

                    CheckerPos newMove(m_NextRightCheckerPos);
                    GetAllPossibleTurns(deck, newMove, excepts, result, false);

                }
            }
        }
        else
        {
            // If we do not step into recursion, then it is new possible move position
            // From start checker position
            if(isInitialPosition)
            {
                result->Push(m_RightCheckerPos);
                excepts.Push(m_RightCheckerPos);
            }
        }
    }
}



void CornersLogic::VAddView(SharedPtr<IGameView> pView, NodeId nodeId)
{
    BaseGameLogic::VAddView(pView, nodeId);

    SharedPtr<CornersHumanView> view = DynamicCast<CornersHumanView>(pView);
    if (view.NotNull())
    {
        // New local human is attached to the game
        m_HumanPlayersAttached++;
    }
}


