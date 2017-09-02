#include "CornersStd.h"
#include <GameEngine/GameLogic/BaseGameLogic.h>

#include "CheckerFactory.h"

const unsigned int CheckerFactory::CHECKER_POOL_SIZE = 3000;
const unsigned int CheckerFactory::DECK_POOL_SIZE = 1000;

CheckerFactory::CheckerFactory(CornersLogic* pCornersLogic) :
	m_pCornersGameLogic(pCornersLogic),
	m_NextFreeDeckId(0),
	m_NextFreeCheckerkId(0)
{

}

CheckerInfo* CheckerFactory::CreateChecker(const String& id, CheckerPos pos, int stepLeft, PlayerType playerType, Node* pNode)
{
	const Vector2& maxFieldSize = m_pCornersGameLogic->GetMaxFieldSize();
	bool result = false;

	if (pos.GetPos().x_ <= maxFieldSize.x_ && pos.GetPos().y_ <= maxFieldSize.y_ &&
		//stepLeft > 0 &&
		(playerType == PlayerType::FIRST_PLAYER || playerType == PlayerType::SECOND_PLAYER) &&
		 pNode != NULL)
	{
		CheckerInfo* newInfo = TakeCheckerFromPool(id);
		if (newInfo)
		{
			newInfo->SetId(id);
			newInfo->SetCheckerPos(pos);
			newInfo->SetLeftStep(stepLeft);
			newInfo->SetPlayerType(playerType);
			newInfo->SetNode(pNode);
			return newInfo;
		}
		else
		{
			result = false;
		}
	}

	if(!result)
	{
		URHO3D_LOGERROR("Failed to factorize Checker!");
	}

	return nullptr;
}

DeckInfo * CheckerFactory::CreateDeck(const String& id, const PODVector<CheckerInfo*>& checkers)
{
	if (checkers.Size() > 0)
	{
		DeckInfo* newDeck = TakeDeckFromPool(id);
		if (newDeck)
		{
			for (unsigned int i = 0; i < checkers.Size(); i++)
			{
				newDeck->SetId(id);
				newDeck->AddChecker(checkers[i]);
			}

			return newDeck;
		}
	}
	else
	{
		URHO3D_LOGERROR("Failed to factorize DeckInfo!");
	}

	return NULL;
}

DeckInfo * CheckerFactory::MoveCheckedInDeck(const String& id, DeckInfo * deck, const CheckerPos & checkerPos, const CheckerPos & newCheckerPos)
{
	if (!deck)
	{
		return nullptr;
	}



	return nullptr;
}

DeckInfo * CheckerFactory::AppendDeck(DeckInfo * deck, const PODVector<CheckerInfo*>& checkers)
{
	if (checkers.Size() > 0)
	{
		DeckInfo* newDeck = deck;
		if (newDeck)
		{
			for (unsigned int i = 0; i < checkers.Size(); i++)
			{
				newDeck->AddChecker(checkers[i]);
			}

			return newDeck;
		}
	}
	else
	{
		URHO3D_LOGERROR("Failed to factorize DeckInfo!");
	}

	return NULL;
}

void CheckerFactory::ResetPool()
{
	m_NextFreeCheckerkId = 0;
	m_CheckerPool.Clear();
	m_ActiveCheckers.Clear();
	m_InactiveCheckers.Clear();

	m_NextFreeDeckId = 0;
	m_DecksPool.Clear();
	m_ActiveDecks.Clear();
	m_InactiveDecks.Clear();
}

CheckerInfo* CheckerFactory::TakeCheckerFromPool(const String& id)
{
	CheckerInfo* result = NULL;

	URHO3D_LOGDEBUG("ActiveCheckerPool Size = " + String(m_ActiveCheckers.Size()));

	if (m_ActiveCheckers.Size() >= CHECKER_POOL_SIZE)
	{
		URHO3D_LOGERROR("CheckPool is FULL !");
		return result;
	}
	else
	{
		if (m_InactiveCheckers.Size() > 0)
		{
			result = m_InactiveCheckers.Front().second_;
			m_InactiveCheckers.Erase(result->GetId());
			return result;
		}
		else
		{
			m_CheckerPool[id.ToHash()] = CheckerInfo(id);
			m_ActiveCheckers[id.ToHash()] = &m_CheckerPool[id.ToHash()];
			result = m_ActiveCheckers[id.ToHash()];
		}
	}

	URHO3D_LOGDEBUG("CheckPool size is = " + String(m_ActiveCheckers.Size()));

	return result;
}

bool CheckerFactory::PutCheckerInPool(CheckerInfo* checkerInfo)
{
	if (m_ActiveCheckers.Contains(checkerInfo->GetId()))
	{
		m_ActiveCheckers.Erase(checkerInfo->GetId());
		m_InactiveCheckers.Insert(MakePair<unsigned int, CheckerInfo*>(checkerInfo->GetId(), checkerInfo));
	}
	else
	{
		URHO3D_LOGERROR("Can't put in CheckerFactory pool old CheckerInfo ! Reason: m_ActiveCheckers does not contain old CheckerInfo !");
	}

	return false;
}

DeckInfo* CheckerFactory::TakeDeckFromPool(const String& id)
{
	DeckInfo* result = NULL;

	if (m_ActiveDecks.Size() >= DECK_POOL_SIZE)
	{
		for(auto it = m_ActiveDecks.Begin(); it != m_ActiveDecks.End(); it++)
		{
			DeckInfo* pDeckInfo = it->second_;
			m_InactiveDecks.Insert(MakePair<unsigned int, DeckInfo*>(pDeckInfo->GetId(), pDeckInfo));
		}
		m_ActiveDecks.Clear();
		URHO3D_LOGERROR("DeckPool is FULL !");
		return result;
	}

	if (m_InactiveDecks.Size() > 0)
	{
		result = m_InactiveDecks.Front().second_;
		m_InactiveDecks.Erase(result->GetId());
		String oldId = String(result->GetId());
		m_ActiveDecks[oldId.ToHash()] = result;
		return result;
	}
	else
	{
		m_DecksPool[id.ToHash()] = DeckInfo(id);
		m_ActiveDecks[id.ToHash()] = &m_DecksPool[id.ToHash()];
		result = m_ActiveDecks[id.ToHash()];
	}

	URHO3D_LOGDEBUG("DeckPool size is = " + String(m_ActiveDecks.Size()));

	return result;
}

bool CheckerFactory::PutDeckInPool(DeckInfo* deckInfo)
{
	if (m_ActiveDecks.Contains(deckInfo->GetId()))
	{
		m_ActiveDecks.Erase(deckInfo->GetId());
		m_InactiveDecks.Insert(MakePair<unsigned int, DeckInfo*>(deckInfo->GetId(), deckInfo));
	}
	else
	{
		URHO3D_LOGERROR("Can't put in CheckerFactory pool old DeckInfo ! Reason: m_ActiveDecks does not contain old DeckInfo !");
	}

	return false;
}


Matrix4 CheckerPos::CalculateCheckerPos3D(const CheckerPos & checkerPos)
{
    Matrix4 result;
    Vector3 newPos3D = Vector3(checkerPos.GetPos().x_ * 2 - 1, 0.2f, checkerPos.GetPos().y_ * 2 - 1);
    result.SetTranslation(newPos3D);
    return result;
}

CheckerPos CheckerPos::CalculateCheckedPosFrom3D(const Vector3& pos)
{
    return CheckerPos(Vector2((pos.x_ + 1) / 2, (pos.z_ + 1) / 2));
}


bool DeckInfo::AddChecker(CheckerInfo* checker)
{
	if (!IsContainChecker(checker))
	{
		m_DeckByCheckerPos[checker->GetCheckerPos().GetPos().ToString()] = checker;

		PlayerType playerType = checker->GetPlayerType();
		m_DeckByPlayers[playerType].Push(checker);
	}
	else
	{
		URHO3D_LOGERROR("Failed to add checker in Deck ! ");
	}

	return false;
}

bool DeckInfo::MoveChecker(CheckerInfo* checker, CheckerPos newPos)
{

	m_DeckByCheckerPos[newPos.GetPos().ToString()] = checker;
	return true;
}

bool DeckInfo::RemoveChecker(const CheckerPos& pos)
{
	return m_DeckByCheckerPos.Erase(pos.GetPos().ToString());
}

bool DeckInfo::IsContainChecker(CheckerInfo* checker)
{
	const CheckerPos& checkerPos = checker->GetCheckerPos();
	return m_DeckByCheckerPos.Contains(checkerPos.GetPos().ToString());
}
