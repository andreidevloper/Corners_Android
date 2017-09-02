#include "GameEngineStd.h"
#include "IGameView.h"

const GameViewId INVALID_GAME_VIEW_ID = M_MAX_UNSIGNED;
const NodeId INVALID_NODE_ID = M_MAX_UNSIGNED;

IGameView::IGameView(Context* context) : Object(context)
{

}

IGameView::~IGameView()
{

}
