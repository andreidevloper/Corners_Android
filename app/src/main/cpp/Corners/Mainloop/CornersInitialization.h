#pragma once

#include <GameEngine/Mainloop/Initialization.h>

struct PlayerOptions
{
	PlayerOptions() : m_Id(-1),
                      m_Wins(0),
					  m_Loses(0),
					  m_TotalPlays(0)
	{

	}

    int m_Id;
	String m_PlayerName;
	int m_Wins;
	int m_Loses;
	int m_TotalPlays;
};


class CornersGameOptions : public BaseGameOptions
{
public:
	const PlayerOptions& GetPlayerOptions() { return m_PlayerOptions; }

	inline void SetPlayerName(String name) { m_PlayerOptions.m_PlayerName = name; }
	inline void SetWins(int wins) { m_PlayerOptions.m_Wins = wins; m_PlayerOptions.m_TotalPlays++; }
	inline void SetLoses(int loses) { m_PlayerOptions.m_Loses = loses; m_PlayerOptions.m_TotalPlays++; }

protected:
	PlayerOptions m_PlayerOptions;
};