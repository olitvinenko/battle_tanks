#pragma once

#include "GameStateBase.h"

class GameStatesController;

class MainMenuState final : public GameStateBase
{
public:
	explicit MainMenuState(GameStatesController* controller);

	void OnEnter() override;
};