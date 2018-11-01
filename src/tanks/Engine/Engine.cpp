#include "Engine.h"
#include "Base/IInput.h"
#include "Base/IWindow.h"
#include "Vector2.h"

Engine::Engine(IWindow* window, IClipboard* clipboard, IInput* input, IRender* render)
	: m_window(window)
	, m_clipboard(clipboard)
	, m_input(input)
	, m_running(false)
	, m_rendering(render)
{
	m_loop.Add<IFixedUpdatable>(this);
	m_loop.Add<IUpdatable>(this);
	m_loop.Add<IRenderable>(this);
}

void Engine::Launch()
{
	if (m_running)
		return;

	m_running = true;

	m_loop.Start();
	while (m_running)
	{
		if (m_window->ShouldClose())
			Shutdown();

		m_loop.Tick();
	}
}

void Engine::Shutdown()
{
	m_running = false;
}

Engine::~Engine()
{
	m_loop.Remove<IFixedUpdatable>(this);
	m_loop.Remove<IUpdatable>(this);
	m_loop.Remove<IRenderable>(this);
}

void Engine::Update(float realDeltaTime)
{
	// read platform input
	m_input->Read();

	m_statesController.DoOnUpdate(realDeltaTime);


	Vector2 pos = m_input->GetMousePosition();

	auto& l = m_input->GetCharacters();

	if (!l.empty())
	{
		for (auto& c : l)
			std::cout << c << " ";

		std::cout << std::endl;
	}


	// coroutines.. animations..

	m_statesController.DoOnLateUpdate(realDeltaTime);

	// clear platform input
	m_input->Clear();
}


void Engine::FixedUpdate(float fixedDeltaTime)
{
	// physics :: update

	m_statesController.DoOnFixedUpdate(fixedDeltaTime);
}


void Engine::Render(float interpolation) const
{
	// preRender ( collect renderables and order  )

	// render ( interpolation )

	// postRender



	// renderEngine::render(interpolation)
	// uiEngine :: render(interpolation)

	//should be tha last one
	m_window->SwapBuffers();
}
