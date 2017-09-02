#pragma once

#include "IGameLogic.h"

#include "IGameView.h"

typedef List<SharedPtr<IGameView>> GameViewList;

#include "IScreenElement.h"

typedef List<SharedPtr<IScreenElement>> ScreenElementList;

#include "Controllers/IPointerHandler.h"
#include "Controllers/IKeyboardHandler.h"

#include "Audio/IAudio.h"
#include "Audio/IAudioBuffer.h"

#include "Database/ISqlCommand.h"

typedef List<IAudioBuffer *> AudioBufferList;

