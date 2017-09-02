#pragma once

#include <GameEngine/Database/DatabaseSubsystem.h>

// Local player always has ID = 2
#define PLAYER_ID 2

// Guest ID
#define GUEST_PLAYER_ID 1


struct PlayerData
{
	 int m_Id;
	 String m_Username;
	 int m_Wins;
	 int m_Loses;
};

class CornersDatabase : public DatabaseSubsystem
{
	URHO3D_OBJECT(CornersDatabase, DatabaseSubsystem)

public:
	CornersDatabase(Context* context);

	DbResult CreatePlayersTable();
	const PlayerData& CreatePlayerData(String username, int wins, int loses);
	const PlayerData& GetPlayerDataIfExist(int id);
	DbResult UpdatePlayerUsername(int id, String username);
	 DbResult UpdateStatistics(int id, int wins, int loses);
	 // Guest
	const PlayerData& CreateGuestData();
	const PlayerData& GetGuestDataIfExist();
	DbResult UpdateGuestStatistics(int wins, int loses);

	const PlayerData& GetPlayerData() const { return m_PlayerData; }
	const PlayerData& GetGuestData() const { return m_GuestData; }

protected:
	// Database's delegates
	void PlayerTrySaveDataDelegate(StringHash eventType, VariantMap& eventData);


private:
	DbResult InsertPlayer(String username, int wins, int loses);
	DbResult UpdatePlayer(int id, String username, int wins, int loses);


	DbResult SelectPlayer(int id);
	DbResult SelectPlayer(String username);
	DbResult SelectAllPlayers();

private:
	 PlayerData m_PlayerData;
	 PlayerData m_GuestData;
};

