#pragma once

#include "Rendering/RenderingEngine.h"
#include "Rendering/IDrawable.h"
#include "WidgetBase.h"

#include <memory>
#include "Base/IInput.h"
#include "UIWindow.h"
#include <algorithm>

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

// 1. EditorLayout
// 2. GameLayout
// 3. LightingLayout
// 4. GuiLayout
// ....
struct ILayout : public IDrawable
{
	virtual ~ILayout() = 0;
};

inline ILayout::~ILayout() = default;

//TODO: navigation stack
class UIDialogsManager final : public WidgetBase
{
public:
	UIDialogsManager(std::shared_ptr<RenderingEngine> renderingEngine)
		: WidgetBase(nullptr, renderingEngine->GetTextureManager())
		, m_menuBack(std::make_shared<WidgetBase>(this, renderingEngine->GetTextureManager()))
	{
		const float width = renderingEngine->GetPixelWidth();
		const float height = renderingEngine->GetPixelHeight();

		m_menuBack->SetTexture("gui_splash", false);
		m_menuBack->SetDrawBorder(false);
		m_menuBack->SetTextureStretchMode(StretchMode::Fill);
		m_menuBack->Resize(width, height);

		Resize(width, height);

		Push(MainMenuDialog::Make(renderingEngine));
	}

	void Push(std::shared_ptr<WidgetBase> widget)
	{
		m_navTransitionStart = GetTransitionTarget();
		m_navTransitionTime = .5f;

		if (!m_navStack.empty())
			m_navStack.back()->SetEnabled(false);

		m_navStack.push_back(widget);
		OnSize(GetWidth(), GetHeight());
	}

	void Update(float deltaTime) override
	{
		deltaTime *= 100.0f/*_conf.sv_speed.GetFloat()*/ / 100.0f;

		if (m_navTransitionTime > 0)
		{
			m_navTransitionTime = std::max(0.f, m_navTransitionTime - deltaTime);
			OnSize(GetWidth(), GetHeight());
		}
	}

	void Pop()
	{
		
	}

	void Pop(std::shared_ptr<WidgetBase*> widget)
	{
		
	}

	void Draw(DrawingContext& dc, float interpolation) const override
	{
		m_menuBack->Draw(dc, interpolation);
		//TODO:: draw only last dialog?

		for (auto it = m_navStack.begin(); it != m_navStack.end(); ++it)
			(*it)->Draw(dc, interpolation);
	}

	~UIDialogsManager()
	{
		m_menuBack.reset();
		m_navStack.clear();
	}

protected:
	void OnSize(float width, float height) override
	{
		m_menuBack->Resize(width, height);

		float transition = (1 - std::cos(PI * m_navTransitionTime / 0.5f)) / 2;
		float top = std::floor(m_navTransitionStart * transition + GetTransitionTarget() * (1 - transition));

		std::cout << top << std::endl;

		for (auto wnd : m_navStack)
		{
			wnd->Move(floorf((width - wnd->GetWidth()) / 2), top);
			top += wnd->GetHeight() + 100.0f;
		}
	}

private:
	float GetTransitionTarget() const
	{
		return (GetHeight() + (m_navStack.empty() ? 0 : m_navStack.back()->GetHeight())) / 2 - GetNavStackSize();
	}

	float GetNavStackSize() const
	{
		float navStackHeight = 0;
		if (!m_navStack.empty())
		{
			for (const auto& wnd : m_navStack)
			{
				navStackHeight += wnd->GetHeight();
			}
			navStackHeight += static_cast<float>(m_navStack.size() - 1) *  100.0f;
		}
		return navStackHeight;
	}

	float m_navTransitionTime = 0;
	float m_navTransitionStart = 0;

	std::shared_ptr<WidgetBase> m_menuBack;
	std::vector<std::shared_ptr<WidgetBase>> m_navStack;
};

class LayoutManager final
	: public IDrawable
	, IInputListener
	, IWindowListener
{
public:
	explicit LayoutManager(std::shared_ptr<RenderingEngine> re, std::shared_ptr<IInput> input);
	~LayoutManager();

	void Initialize();

	// IDrawable
	int GetOrder() const override;
	void Draw(DrawingContext& dc, float interpolation) const override;

	void Update(float deltaTime);

	void Push(std::shared_ptr<WidgetBase> widget)
	{
		m_openedScreens.push_back(widget);
	}

	std::shared_ptr<WidgetBase> Pop()
	{
		std::shared_ptr<WidgetBase> popped = m_openedScreens.back();
		m_openedScreens.pop_back();
		return popped;
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
