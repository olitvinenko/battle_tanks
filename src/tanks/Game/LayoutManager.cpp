#include "LayoutManager.h"
#include "RenderOrder.h"
#include "Rendering/DrawingContext.h"

LayoutManager::LayoutManager(std::shared_ptr<RenderingEngine> re, std::shared_ptr<IInput> input)
	: m_renderingEngine(re)
	, m_input(input)
{
	m_renderingEngine->GetScheme().RegisterDrawable(*this);

	m_input->AddListener(this);
}

LayoutManager::~LayoutManager()
{
	m_input->RemoveListener(this);

	m_renderingEngine->GetScheme().UnegisterDrawable(*this);
}

int LayoutManager::GetOrder() const
{
	return static_cast<int>(RenderOrder::GUI);
}

void LayoutManager::Draw(DrawingContext& dc, float interpolation) const
{
	m_renderingEngine->SetMode(INTERFACE);

	if (!m_openedScreens.empty())
	{
		m_openedScreens.top()->Draw(dc, interpolation);
	}

	math::RectFloat dst = { m_lastPointerLocation.x - 4, m_lastPointerLocation.y - 4,m_lastPointerLocation.x + 4, m_lastPointerLocation.y + 4 };
	dc.DrawSprite(&dst, 0U, 0xffffffff, 0U);

	//get top from stack and render
}

void LayoutManager::Update(float deltaTime)
{
	if (m_openedScreens.empty())
		return;

	m_openedScreens.top()->Update(deltaTime);
}
