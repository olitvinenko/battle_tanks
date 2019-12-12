#pragma once
#include <memory>
#include <set>

#include "GameContextBase.h"

class AppStateListener;

class AppState
{
public:
	AppState();
	~AppState();

	GameContextBase* GetGameContext() const { return _gameContext.get(); }
	void SetGameContext(std::unique_ptr<GameContextBase> gameContext);

private:
	friend class AppStateListener;

	std::unique_ptr<GameContextBase> _gameContext;
	std::set<AppStateListener*> _appStateListeners;

    AppState(const AppState&) = delete;
    void operator=(const AppState&) = delete;
};
