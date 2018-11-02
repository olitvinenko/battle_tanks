#pragma once

#include "Rendering/RenderingEngine.h"
#include "Rendering/IDrawable.h"
#include "WidgetBase.h"

#include <stack>
#include <memory>


class LayoutManager final : public IDrawable
{
public:
	explicit LayoutManager(std::shared_ptr<RenderingEngine> re);
	~LayoutManager();

	// IDrawable
	int GetOrder() const override;
	void Draw(DrawingContext& dc, float interpolation) const override;

	void Update(float deltaTime);

	void Test()
	{
		auto w = std::make_shared<WidgetBase>(nullptr, m_renderingEngine->GetTextureManager());
		
		w->Resize(m_renderingEngine->GetPixelWidth(), m_renderingEngine->GetPixelHeight());

		w->SetTexture("gui_splash", false);
		w->SetDrawBorder(false);
		w->SetTextureStretchMode(StretchMode::Fill);

		WidgetBase* c = new WidgetBase(w.get(), m_renderingEngine->GetTextureManager());

		c->Move(m_renderingEngine->GetPixelWidth() / 2, m_renderingEngine->GetPixelHeight() / 2);
		c->Resize(m_renderingEngine->GetPixelWidth() / 2, m_renderingEngine->GetPixelHeight() / 2);
		c->SetTexture("gui_splash", false);
		c->SetDrawBorder(false);
		c->SetTextureStretchMode(StretchMode::Stretch);

		m_openedScreens.push(w);
	}


private:
	std::shared_ptr<RenderingEngine> m_renderingEngine;
	std::stack<std::shared_ptr<WidgetBase>> m_openedScreens;

	//std::unordered_map<ScreenType, std::function<std::shared_ptr<UI::UIWindow>()>> m_dialogs;
};
