#pragma once

#include "GameStateBase.h"

//TODO: integration

class TestState final : public GameStateBase
{
public:
	explicit TestState(std::shared_ptr<GameStatesController> controller);

protected:
	void OnUpdate() override;
	void OnLateUpdate() override;
	void OnFixedUpdate() override;
public:
	void OnEnter() override;
	void OnExit() override;
};

