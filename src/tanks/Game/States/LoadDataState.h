#pragma once

#include "GameStateBase.h"

class GameStatesController;

class LoadDataState final : public GameStateBase
{
public:
	explicit LoadDataState(GameStatesController* controller);

	void OnEnter() override;

protected:
	void OnUpdate(float deltaTime) override;

private:
	void InitTextureManager();

private:
	volatile bool m_texturesLoaded;
};