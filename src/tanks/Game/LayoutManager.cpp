#include "LayoutManager.h"
#include "RenderOrder.h"
#include "Rendering/DrawingContext.h"

LayoutManager::LayoutManager(std::shared_ptr<RenderingEngine> re, std::shared_ptr<IInput> input)
	: m_renderingEngine(re)
	, m_input(input)
{
	m_renderingEngine->GetScheme().RegisterDrawable(*this);

	m_input->AddListener(this);
	m_renderingEngine->GetWindow()->AddListener(this);
}

LayoutManager::~LayoutManager()
{
	m_input->RemoveListener(this);
	m_renderingEngine->GetWindow()->RemoveListener(this);

	m_renderingEngine->GetScheme().UnegisterDrawable(*this);
}

void LayoutManager::Initialize()
{
	//GUI system
	Push(std::make_shared<UIDialogsManager>(m_renderingEngine));
}

int LayoutManager::GetOrder() const
{
	return static_cast<int>(RenderOrder::GUI);
}

void LayoutManager::Draw(DrawingContext& dc, float interpolation) const
{
	m_renderingEngine->SetMode(INTERFACE);

	for (auto it = m_openedScreens.begin(); it != m_openedScreens.end(); ++it)
		(*it)->Draw(dc, interpolation);

	math::RectFloat dst { m_lastPointerLocation.x - 4, m_lastPointerLocation.y - 4, m_lastPointerLocation.x + 4, m_lastPointerLocation.y + 4 };
	dc.DrawSprite(&dst, 0U, 0xffffffff, 0U);
}

void LayoutManager::Update(float deltaTime)
{
	if (m_openedScreens.empty())
		return;

	m_openedScreens.back()->Update(deltaTime);
}

void LayoutManager::OnFrameBufferChanged(int width, int height)
{
	for (auto& element : m_openedScreens)
		element->Resize(width, height);
}
