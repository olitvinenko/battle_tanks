#pragma once

#include "Rendering/RenderingEngine.h"
#include "Rendering/IDrawable.h"
#include "WidgetBase.h"

#include <stack>
#include <memory>
#include "Base/IInput.h"

class MainMenuScreen : public WidgetBase
{
public:
	MainMenuScreen(const TextureManager& textures):WidgetBase(nullptr, textures)
	{}


protected:
	void OnSize(float width, float height) override
	{
		//for (WidgetBase* wnd : m_children)
		//	wnd->Move((width - wnd->GetWidth()) / 2, wnd->GetY());
	}

	void OnMove(float x, float y) override
	{

	}

public:
	static std::shared_ptr<MainMenuScreen> Make(std::shared_ptr<RenderingEngine> re)
	{
		const float buttonWidth = 200;
		const float buttonHeight = 128;

		auto panel = std::make_shared<MainMenuScreen>(re->GetTextureManager());
		panel->Resize(640, 300);
		panel->Move(re->GetPixelWidth() / 2 - 320, 0);
		panel->SetTexture("gui_splash", false);

		float y = panel->GetHeight() - buttonHeight;

		WidgetBase* button = new WidgetBase(panel.get(), re->GetTextureManager());
		button->Move(0, y);
		button->SetTexture("ui/button", true);
		button->Resize(buttonWidth, buttonHeight);

		button = new WidgetBase(panel.get(), re->GetTextureManager());
		button->Move((panel->GetWidth() - buttonWidth) / 2, y);
		button->SetTexture("ui/button", true);
		button->Resize(buttonWidth, buttonHeight);

		button = new WidgetBase(panel.get(), re->GetTextureManager());
		button->Move(panel->GetWidth() - buttonWidth, y);
		button->SetTexture("ui/button", true);
		button->Resize(buttonWidth, buttonHeight);

		button = new WidgetBase(panel.get(), re->GetTextureManager());
		button->Move((panel->GetWidth() - buttonWidth) / 2, 0);
		button->SetTexture("ui/button", true);
		button->Resize(buttonWidth, buttonHeight);

		button = new WidgetBase(panel.get(), re->GetTextureManager());
		button->Move((panel->GetWidth() - buttonWidth), y * 2);
		button->SetTexture("ui/button", true);
		button->Resize(buttonWidth, buttonHeight);

		button = new WidgetBase(panel.get(), re->GetTextureManager());
		button->Move((panel->GetWidth() - buttonWidth), y * 2 + buttonHeight);
		button->SetTexture("ui/button", true);
		button->Resize(buttonWidth, buttonHeight);

		return panel;
	}
};


class LayoutManager final
	: public IDrawable
	, IInputListener
	, IWindowListener
{
public:
	explicit LayoutManager(std::shared_ptr<RenderingEngine> re, std::shared_ptr<IInput> input);
	~LayoutManager();

	// IDrawable
	int GetOrder() const override;
	void Draw(DrawingContext& dc, float interpolation) const override;

	void Update(float deltaTime);

	void Test()
	{
		auto back = std::make_shared<WidgetBase>(nullptr, m_renderingEngine->GetTextureManager());
		back->Resize(m_renderingEngine->GetPixelWidth(), m_renderingEngine->GetPixelHeight());
		back->SetTexture("gui_splash", false);
		back->SetDrawBorder(false);
		back->SetTextureStretchMode(StretchMode::Fill);
		//m_openedScreens.push(back);

		auto screen = MainMenuScreen::Make(m_renderingEngine);

		m_openedScreens.push_back(screen);

		//screen->Resize(m_renderingEngine->GetWindow()->GetPixelWidth(), m_renderingEngine->GetWindow()->GetPixelHeight());
	}

private:
	//TODO::
	//> IInputListener
	void OnMouseButtonDown(MouseButton button) override
	{
		std::cout << "OnMouseButtonDown" << std::endl;
	}

	void OnMouseButtonUp(MouseButton button) override
	{
		std::cout << "OnMouseButtonUp" << std::endl;
	}

	void OnMousePosition(float x, float y) override
	{
		std::cout << "OnMousePosition" << std::endl;

		m_lastPointerLocation = Vector2(x, y);
	}

	void OnMouseScrollOffset(float x, float y) override
	{
		std::cout << "OnMouseScrollOffset" << std::endl;
	}

	void OnCharacter(char character) override
	{
		std::cout << "OnCharacter " << character << std::endl;
	}

	void OnKeyDown(Key key) override
	{
		std::cout << "OnKeyDown" << std::endl;
	}

	void OnKey(Key key) override
	{
		std::cout << "OnKey" << std::endl;
	}

	void OnKeyUp(Key key) override
	{
		std::cout << "OnKeyUp" << std::endl;
	}

	//-> IWindowListener
	void OnFrameBufferChanged(int width, int height) override;

	void OnSizeChanged(int width, int height) override
	{
		std::cout << "OnSizeChanged" << std::endl;
	}

	void OnClosed() override
	{
		std::cout << "OnClosed" << std::endl;
	}

private:
	std::shared_ptr<RenderingEngine> m_renderingEngine;
	std::shared_ptr<IInput> m_input;

	std::vector<std::shared_ptr<WidgetBase>> m_openedScreens;

	Vector2 m_lastPointerLocation;
};
