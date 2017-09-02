#pragma once

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_PLAYER_SAVE_DATA - This event is sent out (typically by UI) when a data about player is need to save
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_PLAYER_SAVE_DATA, SAVE_PLAYER_DATA)
{
	URHO3D_PARAM(P_USERNAME, P_USERNAME_VALUE); // String
}
