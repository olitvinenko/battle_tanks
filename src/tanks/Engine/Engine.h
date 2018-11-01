#pragma once

#include "ThreadPool.h"
#include "GameStatesController.h"
#include "GameLoop.h"

#include "Rendering/RenderingEngine.h";

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
	explicit Engine(IWindow* window, IClipboard* clipboard, IInput* input, IRender* render);

	void Update(float realDeltaTime) override;
	void FixedUpdate(float fixedDeltaTime) override;
	void Render(float interpolation) const override;

	void Launch();
	void Shutdown();

	~Engine();

	GameStatesController& GetStatesController() { return m_statesController; }
	RenderingEngine& GetRender() { return m_rendering; }

private:
	IWindow* m_window;
	IClipboard* m_clipboard;
	IInput* m_input;

	GameLoop m_loop;

	ThreadPool m_threadPool;
	GameStatesController m_statesController;

	RenderingEngine m_rendering;

	bool m_running;
};
