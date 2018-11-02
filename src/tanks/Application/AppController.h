#pragma once

#include "AppState.h"
#include "GameLoop.h"

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ConsoleBuffer;
}

class AppController : public IFixedUpdatable
{
public:
	AppController(GameLoop &loop, FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger);
	~AppController();

	AppState& GetAppState();
	FileSystem::IFileSystem& GetFs() { return m_fs; }
	UI::ConsoleBuffer& GetLogger() { return m_logger; }

	void FixedUpdate(float fixedDeltaTime) override;
	void Exit() const;

private:

	AppState m_appState;
	FileSystem::IFileSystem &m_fs;
	UI::ConsoleBuffer &m_logger;
	GameLoop &m_loop;
};
