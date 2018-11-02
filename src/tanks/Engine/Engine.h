#pragma once

#include "ThreadPool.h"
#include "GameStatesController.h"
#include "GameLoop.h"

#include "Rendering/RenderingEngine.h";

#include <memory>

struct IClipboard;
struct IWindow;
struct IInput;

namespace FileSystem
{
	class IFileSystem;
}


class Engine final
	: public IFixedUpdatable
	, public IUpdatable
	, public IRenderable
{
public:
	Engine(std::shared_ptr<IWindow> window, std::shared_ptr<IClipboard> clipboard, std::shared_ptr<IInput> input, std::shared_ptr<FileSystem::IFileSystem> fileSystem, IRender* render, int layersCount);

	void Update(float realDeltaTime) override;
	void FixedUpdate(float fixedDeltaTime) override;
	void Render(float interpolation) const override;

	void Launch();
	void Shutdown();

	~Engine();

	GameStatesController& GetStatesController() { return m_statesController; }

	std::shared_ptr<FileSystem::IFileSystem> GetFileSystem() { return m_fileSystem; }
	std::shared_ptr<RenderingEngine> GetRenderingEngine() { return m_rendering; }

	GameLoop& GetLoop() { return m_loop; }

private:
	// view
	std::shared_ptr<IWindow> m_window;
	std::shared_ptr<IClipboard> m_clipboard;
	std::shared_ptr<IInput> m_input;

	// engines // unique?
	std::shared_ptr<RenderingEngine> m_rendering;

	// systems
	std::shared_ptr<FileSystem::IFileSystem> m_fileSystem;
	std::shared_ptr<ThreadPool> m_threadPool;

	// game
	GameLoop m_loop;	
	GameStatesController m_statesController;
	

	bool m_running;
};
