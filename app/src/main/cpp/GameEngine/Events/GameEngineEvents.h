#pragma once

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_START_GAME - This event is sent out when  game/simulation is started (typically after main menu)
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_START_GAME, START_GAME)
{
}

//---------------------------------------------------------------------------------------------------------------------
// EVENT_DATA_ENVIRONMENT_LOADED - This event is sent out when game's/simulation's scene environment is successfully loaded
//---------------------------------------------------------------------------------------------------------------------

URHO3D_EVENT(EVENT_DATA_ENVIRONMENT_LOADED, ENVIRONMENT_LOADED)
{
}

//----------------------------------------------------------------------------------------------------------------
// EVENT_DATA_ENABLE_SOUND - this event is send to AudioSybsystem if sound parameters is changed
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_ENABLE_SOUND, ENABLE_SOUND)
{
}

//----------------------------------------------------------------------------------------------------------------
// EVENT_DATA_MASTER_VOLUME - this event is send to AudioSybsystem if sound parameters is changed
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_MASTER_VOLUME, MASTER_VOLUME)
{
}

//----------------------------------------------------------------------------------------------------------------
// EVENT_DATA_SFX_VOLUME - this event is send to AudioSybsystem if sound parameters is changed
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_SFX_VOLUME, SFX_VOLUME)
{
}

//----------------------------------------------------------------------------------------------------------------
// EVENT_DATA_VOICE_VOLUME - this event is send to AudioSybsystem if sound parameters is changed
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_VOICE_VOLUME, VOICE_VOLUME)
{
}


//----------------------------------------------------------------------------------------------------------------
// EVENT_DATA_MUSIC_VOLUME - this event is send to AudioSybsystem if sound parameters is changed
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_MUSIC_VOLUME, MUSIC_VOLUME)
{
}


//----------------------------------------------------------------------------------------------------------------
// EVENT_DATA_AMBIENT_VOLUME - this event is send to humanview if sound parameters is changed
//---------------------------------------------------------------------------------------------------------------------
URHO3D_EVENT(EVENT_DATA_AMBIENT_VOLUME, AMBIENT_VOLUME)
{
}