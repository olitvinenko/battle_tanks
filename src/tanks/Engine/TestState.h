#pragma once

#include "GameStateBase.h"

//TODO: integration

class TestState final : public GameStateBase
{
public:
	explicit TestState(std::shared_ptr<GameStatesController> controller);

protected:
	void OnUpdate(float deltaTime) override;
	void OnLateUpdate(float deltaTime) override;
	void OnFixedUpdate(float fixedDeltaTime) override;
public:
	void OnEnter() override;
	void OnExit() override;
};

