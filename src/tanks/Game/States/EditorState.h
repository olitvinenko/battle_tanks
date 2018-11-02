#pragma once

#include "GameStateBase.h"

class GameStatesController;

class EditorState final : public GameStateBase
{
public:
	explicit EditorState(GameStatesController* controller);
};