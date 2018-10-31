#pragma once

#include <memory>

class GameStatesController;

struct GameStateBase
{
protected:
	std::weak_ptr<GameStatesController> m_controller;

	virtual void OnUpdate(float deltaTime);
	virtual void OnLateUpdate(float deltaTime);
	virtual void OnFixedUpdate(float fixedDeltaTime);

	explicit GameStateBase(std::shared_ptr<GameStatesController> controller);

public:
	virtual void OnEnter();
	virtual void OnEnterForeground();
	virtual void OnEnterBackground();

	virtual void OnExit();
	virtual void OnExitForeground();
	virtual void OnExitBackground();

	virtual void OnForegroundUpdate(float deltaTime);
	virtual void OnBackgroundUpdate(float deltaTime);

	virtual void OnForegroundLateUpdate(float deltaTime);
	virtual void OnBackgroundLateUpdate(float deltaTime);

	virtual void OnForegroundFixedUpdate(float fixedDeltaTime);
	virtual void OnBackgroundFixedUpdate(float fixedDeltaTime);

	virtual ~GameStateBase() = 0;
};
