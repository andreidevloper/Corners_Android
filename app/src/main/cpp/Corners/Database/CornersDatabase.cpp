#include "CornersStd.h"

#include <GameEngine/Events/Database/DatabaseEvent.h>

#include "CornersDatabase.h"

CornersDatabase::CornersDatabase(Context* context) : DatabaseSubsystem(context)
{
	m_PlayerData.m_Username = "Invalid";
	m_PlayerData.m_Id = -1;
	m_PlayerData.m_Wins = -1;
	m_PlayerData.m_Loses = -1;

	m_GuestData.m_Username = "Invalid";
	m_GuestData.m_Id = -1;
	m_GuestData.m_Wins = -1;
	m_GuestData.m_Loses = -1;

    // Database
    SubscribeToEvent(EVENT_DATA_PLAYER_SAVE_DATA, URHO3D_HANDLER(CornersDatabase, PlayerTrySaveDataDelegate));

}

DbResult CornersDatabase::CreatePlayersTable()
{

	return CreateTable(String("Player (id INTEGER PRIMARY KEY, username VARCHAR(64), wins INTEGER, loses INTEGER);"));
}

DbResult CornersDatabase::InsertPlayer(String username, int wins, int loses)
{
	return ExecuteSql(String("INSERT INTO Player (username, wins, loses) VALUES (") + String("'") + username + "'" +
			                     ", " + String(wins) + ", " + String(loses) + ");");
}

DbResult CornersDatabase::UpdatePlayer(int id, String username, int wins, int loses)
{
	return ExecuteSql(String("UPDATE Player SET username = " + String("'") + username + "'" + ", "
	                          "wins = " + String(wins) + ", " +
	                          "loses = " + String(loses) + " WHERE id = " + String(id) + ";"));
}

DbResult CornersDatabase::UpdatePlayerUsername(int id, String username)
{
	m_PlayerData.m_Username = username;
	return ExecuteSql(String("UPDATE Player SET username = " + String("'") + username + "'" + " WHERE id = " + String(id) + ";"));
}

DbResult CornersDatabase::UpdateStatistics(int id, int wins, int loses)
{
	if(id == 1)
	{
		m_GuestData.m_Wins = wins;
		m_GuestData.m_Loses = loses;
	}
	else if (id == 2)
	{
		m_PlayerData.m_Wins = wins;
		m_PlayerData.m_Loses = loses;
	}

	return ExecuteSql(String("UPDATE Player SET wins = " + String(wins) + ", " +
	                          "loses = " + String(loses) + " WHERE id = " + String(id) + ";"));
}

DbResult CornersDatabase::SelectPlayer(int id)
{
	return ExecuteSql(String("SELECT A.id, A.username, A.wins, A.loses FROM Player A WHERE A.id = " + String(id) + ";"));
}

DbResult CornersDatabase::SelectPlayer(String username)
{
	return ExecuteSql(String("SELECT A.id, A.username, A.wins, A.loses FROM Player A WHERE A.username = '" + username + "';"));
}

DbResult CornersDatabase::SelectAllPlayers()
{
	return ExecuteSql(String("SELECT A.username, A.wins, A.loses FROM Player A;"));
}

const PlayerData& CornersDatabase::CreatePlayerData(String username, int wins, int loses)
{
	DbResult result = InsertPlayer(username, wins, loses);
	return GetPlayerDataIfExist(PLAYER_ID);
}

const PlayerData& CornersDatabase::GetPlayerDataIfExist(int id)
{
	DbResult result = SelectPlayer(id);

	const Vector<VariantVector>& rows = result.GetRows();
	if(rows.Size() != 0)
	{
		auto firstRow = rows.Begin()->Begin();
		int id = firstRow->GetInt();
		firstRow++;

		String username = firstRow->GetString();
		firstRow++;

		int wins = firstRow->GetInt();
		firstRow++;

		int loses = firstRow->GetInt();
		firstRow++;

		m_PlayerData.m_Id = id;
		m_PlayerData.m_Username = username;
		m_PlayerData.m_Wins = wins;
		m_PlayerData.m_Loses = loses;
	}

	return m_PlayerData;
}

const PlayerData& CornersDatabase::CreateGuestData()
{
	GetGuestDataIfExist();

	if(m_GuestData.m_Username == "Invalid")
	{
		InsertPlayer("Guest", 0, 0);
	}

	GetGuestDataIfExist();

	return m_GuestData;
}

const PlayerData& CornersDatabase::GetGuestDataIfExist()
{
	DbResult result = SelectPlayer("Guest");
	const Vector<VariantVector>& rows = result.GetRows();
	if(rows.Size() != 0)
	{
		auto firstRow = rows.Begin()->Begin();
		int id = firstRow->GetInt();
		firstRow++;

		String username = firstRow->GetString();
		firstRow++;

		int wins = firstRow->GetInt();
		firstRow++;

		int loses = firstRow->GetInt();
		firstRow++;

		m_GuestData.m_Id = id;
		m_GuestData.m_Username = username;
		m_GuestData.m_Wins = wins;
		m_GuestData.m_Loses = loses;
	}

	return m_GuestData;
}


void CornersDatabase::PlayerTrySaveDataDelegate(StringHash eventType, VariantMap& eventData)
{
    String name = eventData[SAVE_PLAYER_DATA::P_USERNAME].GetString();
    const PlayerData& playerData = GetPlayerDataIfExist(PLAYER_ID);
    if(playerData.m_Username != "Invalid" && !name.Empty() && playerData.m_Username != name)
    {
        UpdatePlayerUsername(2, name);
    }
}