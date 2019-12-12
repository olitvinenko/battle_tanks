#include "DataSource.h"
#include "GuiManager.h"
#include "InputContext.h"
#include "LayoutContext.h"
#include "Window.h"
#include <algorithm>

using namespace UI;

Window::Window(LayoutManager &manager)
  : _manager(manager)
  , _isVisible(true)
  , _isTopMost(false)
  , _isTimeStep(false)
  , _clipChildren(false)
{
}

Window::~Window()
{
	SetTimeStep(false);
	UnlinkAllChildren();
}

void Window::UnlinkAllChildren()
{
	_focusChild.reset();
	_children.clear();
}

void Window::UnlinkChild(Window &child)
{
	if (_focusChild.get() == &child)
		_focusChild = nullptr;
	_children.erase(
		std::remove_if(std::begin(_children), std::end(_children), [&](auto &which) { return which.get() == &child;} ),
		_children.end());
}

void Window::AddFront(std::shared_ptr<Window> child)
{
	_children.push_back(std::move(child));
}

void Window::AddBack(std::shared_ptr<Window> child)
{
	_children.push_front(std::move(child));
}

RectFloat Window::GetChildRect(TextureManager &texman, const LayoutContext &lc, const StateContext &sc, const Window &child) const
{
	return CanvasLayout(child.GetOffset(), child.GetSize(), lc.GetScale());
}

void Window::Move(float x, float y)
{
	_x = x;
	_y = y;
}

void Window::Resize(float width, float height)
{
	if( _width != width || _height != height )
	{
		_width  = width;
		_height = height;
	}
}

void Window::SetTopMost(bool topmost)
{
	_isTopMost = topmost;
}

void Window::SetTimeStep(bool enable)
{
	if( enable != _isTimeStep )
	{
		if( _isTimeStep )
			GetManager().TimeStepUnregister(_timeStepReg);
		else
			_timeStepReg = GetManager().TimeStepRegister(this);
		_isTimeStep = enable;
	}
}

void Window::SetFocus(std::shared_ptr<Window> child)
{
	assert(!child || _children.end() != std::find(_children.begin(), _children.end(), child));
	_focusChild = child;
}

std::shared_ptr<Window> Window::GetFocus() const
{
	return _focusChild;
}

void Window::SetEnabled(std::shared_ptr<DataSource<bool>> enabled)
{
	_enabled = std::move(enabled);
}

bool Window::GetEnabled(const StateContext &sc) const
{
	return _enabled ? _enabled->GetValue(sc) : true;
}

void Window::SetVisible(bool visible)
{
	_isVisible = visible;
}


//
// other
//

void Window::OnTimeStep(LayoutManager &manager, float dt)
{
}


RectFloat UI::CanvasLayout(Vector2 offset, Vector2 size, float scale)
{
	return MakeRectWH(Vec2dFloor(offset * scale), Vec2dFloor(size * scale));
}
