#pragma once

#include "GameStateBase.h"

class GameStatesController;

class PauseState final : public GameStateBase
{
public:
	explicit PauseState(GameStatesController* controller);
};