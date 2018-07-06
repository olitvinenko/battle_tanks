#include "AppState.h"
#include "AppStateListener.h"

AppState::AppState()
{
}

AppState::~AppState()
{
}

void AppState::SetGameContext(std::unique_ptr<GameContextBase> gameContext)
{
	for (AppStateListener *ls: _appStateListeners)
		ls->OnGameContextChanging();
	_gameContext = std::move(gameContext);
	for (AppStateListener *ls: _appStateListeners)
		ls->OnGameContextChanged();
}
