#include "LayoutManager.h"
#include "RenderOrder.h"

LayoutManager::LayoutManager(std::shared_ptr<RenderingEngine> re)
	: m_renderingEngine(re)
{
	m_renderingEngine->GetScheme().RegisterDrawable(*this);
}

LayoutManager::~LayoutManager()
{
	m_renderingEngine->GetScheme().UnegisterDrawable(*this);
}

int LayoutManager::GetOrder() const
{
	return static_cast<int>(RenderOrder::GUI);
}

void LayoutManager::Draw(DrawingContext& dc, float interpolation) const
{
	m_renderingEngine->SetMode(INTERFACE);

	if (m_openedScreens.empty())
		return;

	m_openedScreens.top()->Draw(dc, interpolation);

	//get top from stack and render
}

void LayoutManager::Update(float deltaTime)
{
	if (m_openedScreens.empty())
		return;

	m_openedScreens.top()->Update(deltaTime);
}