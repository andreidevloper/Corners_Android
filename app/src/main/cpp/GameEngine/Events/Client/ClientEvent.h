#pragma once

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_TRY_DISCONNECT - This event is sent out when client tries to disconnect
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_TRY_DISCONNECT, CLIENT_TRY_DISCONNECT)
{
	URHO3D_PARAM(P_IS_RETURN, IS_RETURN_VALUE);  // String
}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_INIT_PROXY_DELEGATES - This event is sent out for subsystems (e.x. playermanager) when client is attached.
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_INIT_PROXY_DELEGATES, INIT_PROXY_DELEGATES)
{

}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_DISCONNECTED - This event is sent out for subsystems (e.x. playermanager) when client is disconnected.
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_DISCONNECTED, CLIENT_DISCONNECTED)
{

}


//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_LOGIN_RESULT - sent by the client to local subsystems (e.x. for mainmenu) that log is successful or not
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_LOGIN_RESULT, CLIENT_LOGIN_RESULT)
{
	URHO3D_PARAM(P_SUCCESS, P_SUCCESS_VALUE); // bool
	URHO3D_PARAM(P_REASON, P_REASON_VALUE);  // if false. String
	URHO3D_PARAM(P_ACCOUNT_ID, P_ACCOUNT_ID_VALUE); // Primary Key (ConnectionId) from DB
}


//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_HANDSHAKE_COMPLETE - This event is sent out when client get successful handshake
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_HANDSHAKE_COMPLETE, CLIENT_HANDSHAKE_COMPLETE)
{

}


//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_TRY_ENTER_IN_LOBBY - this is sent by the game client, in order to try to enter in lobby
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_TRY_ENTER_IN_LOBBY, REQUEST_ENTER_IN_LOBBY)
{

}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_TRY_START_GAME - this is sent by the game client, in order to try to entert in the game
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_TRY_START_GAME, REQUEST_START_GAME)
{

}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME - this is sent by the game client, in order to play again game
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME, TRY_PLAY_AGAIN_GAME)
{

}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME - this is sent by the game client, in order to say that game is finished
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_FINISH_GAME, CLIENT_FINISH_GAME)
{
	 URHO3D_PARAM(P_PLAYER_TYPE, P__PLAYER_TYPE_VALUE); // PlayerType structure (Who has finished the turn ?)
	 URHO3D_PARAM(P_WINNER_PLAYER_NAME, P_WINNER_PLAYER_NAME_VALUE); // String
}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_FINISH_TURN - this is sent by the game client, in order to say that client is finished to do his/her turn
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_CLIENT_FINISH_TURN, CLIENT_FINISH_TURN)
{
	 URHO3D_PARAM(P_PLAYER_TYPE, P_PLAYER_TYPE_VALUE); // PlayerType structure (Who has finished the turn ?)
	 URHO3D_PARAM(P_NEW_POSITION, P_NEW_POSITION_VALUE); // Vector2
	 URHO3D_PARAM(P_OLD_POSITION, P_OLD_POSITION_VALUE); // Vector2
}


//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_CLIENT_LOBBY_INFO_RECIEVED - sent by the client to local subsystems (e.x. for mainmenu) that lobby info is recieved
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_CLIENT_LOBBY_INFO_RECIEVED, CLIENT_LOBBY_INFO_RECIEVED)
{
	URHO3D_PARAM(P_SUCCESS, P_SUCCESS_VALUE); // bool
}





