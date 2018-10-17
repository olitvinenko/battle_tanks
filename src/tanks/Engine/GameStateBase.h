#pragma once

#include <memory>

class GameStatesController;

struct GameStateBase
{
protected:
	std::weak_ptr<GameStatesController> m_controller;

	virtual void OnUpdate();
	virtual void OnLateUpdate();
	virtual void OnFixedUpdate();

	explicit GameStateBase(std::shared_ptr<GameStatesController> controller);

public:
	virtual void OnEnter();
	virtual void OnEnterForeground();
	virtual void OnEnterBackground();

	virtual void OnExit();
	virtual void OnExitForeground();
	virtual void OnExitBackground();

	virtual void OnForegroundUpdate();
	virtual void OnBackgroundUpdate();

	virtual void OnForegroundLateUpdate();
	virtual void OnBackgroundLateUpdate();

	virtual void OnForegroundFixedUpdate();
	virtual void OnBackgroundFixedUpdate();

	virtual ~GameStateBase() = 0;
};
