#pragma once

class AppState;

class AppStateListener
{
public:
	explicit AppStateListener(AppState &appState);
	virtual ~AppStateListener();
	AppState& GetAppState() const { return _appState; }

	virtual void OnGameContextChanging() = 0;
	virtual void OnGameContextChanged() = 0;

private:
	AppState &_appState;

	AppStateListener(const AppStateListener &) = delete;
	void operator=(const AppStateListener &) = delete;
};
