#pragma once

#include "UIWindow.h"
#include "Rendering/RenderingEngine.h"

#include <stack>
#include <unordered_map>
#include <functional>
#include <memory>

enum class DrawingOrder
{
	First,

	Background = First,
	UI,

	Last
};

class LayoutManager final
{
public:
	explicit LayoutManager(std::shared_ptr<RenderingEngine> re);

	//TODO:: pass screen type?
	//TODO:: return shared_ptr with newly created screen ( why? - because of event registering )
	void RegisterScreen(const IDrawable& drawable)
	{
		//TODO:: create ui

		m_renderingEngine->GetScheme().RegisterDrawable(drawable);
	}

	void UnregisterScreen(const IDrawable& drawable)
	{
		//TODO:: destroy

		m_renderingEngine->GetScheme().UnegisterDrawable(drawable);
	}

private:
	std::shared_ptr<RenderingEngine> m_renderingEngine;
	std::stack<std::shared_ptr<UI::UIWindow>> m_openedScreens;
};