#pragma once

#include "GameStateBase.h"

class GameStatesController;

class GameplayState final : public GameStateBase
{
public:
	explicit GameplayState(GameStatesController* controller);
};