#pragma once

#include "Rendering/RenderingEngine.h"
#include "Rendering/IDrawable.h"
#include "WidgetBase.h"

#include <stack>
#include <memory>
#include "Base/IInput.h"
#include "UIWindow.h"

//TODO: navigation stack
class UIDialogsManager final : public WidgetBase
{
public:
	

	UIDialogsManager(float width, float height, const TextureManager& textures)
		: WidgetBase(nullptr, textures)
	{
		Resize(width, height);
	}

	void Push(std::shared_ptr<WidgetBase> widget)
	{
		m_navStack.push_back(widget);
		OnSize(GetWidth(), GetHeight());
	}

	void Pop()
	{
		
	}

	void Pop(std::shared_ptr<WidgetBase*> widget)
	{
		
	}

	void Draw(DrawingContext& dc, float interpolation) const override
	{
		//TODO:: draw only last dialog?

		for (auto it = m_navStack.begin(); it != m_navStack.end(); ++it)
			(*it)->Draw(dc, interpolation);
	}

	~UIDialogsManager()
	{

	}

protected:
	void OnSize(float width, float height) override
	{
		//float top = height/2.0f;
		for (auto wnd : m_navStack)
		{
			wnd->Move(floorf((width - wnd->GetWidth()) / 2), floorf((height - wnd->GetHeight()) / 2));
			//top += wnd->GetHeight() + 100.0f;// _conf.ui_spacing.GetFloat();
		}
	}

private:
	std::vector<std::shared_ptr<WidgetBase>> m_navStack;
};

class MainMenuDialog : public WidgetBase
{
public:
	MainMenuDialog(const TextureManager& textures)
		:WidgetBase(nullptr, textures)
	{}

	static std::shared_ptr<MainMenuDialog> Make(std::shared_ptr<RenderingEngine> re)
	{
		const float buttonWidth = 200;
		const float buttonHeight = 128;

		auto panel = std::make_shared<MainMenuDialog>(re->GetTextureManager());
		panel->Resize(640, 300);

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
		{ // background
			auto back = std::make_shared<WidgetBase>(nullptr, m_renderingEngine->GetTextureManager());
			back->Resize(m_renderingEngine->GetPixelWidth(), m_renderingEngine->GetPixelHeight());
			back->SetTexture("gui_splash", false);
			back->SetDrawBorder(false);
			back->SetTextureStretchMode(StretchMode::Fill);
			m_openedScreens.push_back(back);
			//back->SetVisible(false);
		}

		{
			auto dm = std::make_shared<UIDialogsManager>(m_renderingEngine->GetPixelWidth(), m_renderingEngine->GetPixelHeight(), m_renderingEngine->GetTextureManager());

			auto screen = MainMenuDialog::Make(m_renderingEngine);
			dm->Push(screen);

			m_openedScreens.push_back(dm);
		}
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
		//std::cout << "OnMousePosition" << std::endl;

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
