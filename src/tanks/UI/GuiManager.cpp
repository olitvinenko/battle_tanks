// GuiManager.cpp

#include "GuiManager.h"
#include "UIWindow.h"
#include "Rect.h"
#include "AppController.h"
#include "Rendering/DrawingContext.h"
#include "Desktop.h"
#include <memory>
#include "RenderOrder.h"
#include "Base/MouseButton.h"

namespace UI
{

LayoutManager::LayoutManager(IInput &input, IClipboard &clipboard, TextureManager &texman, AppController& controller)
  : //_input(input)
 // , _clipboard(clipboard)
    _texman(texman)
  , _tsCurrent(_timestep.end())
  , _tsDeleteCurrent(false)
  , _captureCountSystem(0)
  , _focusWnd(nullptr)
  , _hotTrackWnd(nullptr)
  , _desktop(nullptr)
  , _isAppActive(false)
#ifndef NDEBUG
  , _dbgFocusIsChanging(false)
#endif
{
	_desktop.Set(new Desktop(this, controller.GetAppState(), controller.GetFs(), controller.GetLogger()));
}

void LayoutManager::Initialize()
{
	_desktop.Set(new Desktop(this, *m_fs, m_renderingEngine->GetPixelWidth(), m_renderingEngine->GetPixelHeight()));
}

LayoutManager::LayoutManager(std::shared_ptr<RenderingEngine> re, std::shared_ptr<FileSystem::IFileSystem> fs, std::shared_ptr<IInput> input, std::shared_ptr<IClipboard> clipboard)
	: m_renderingEngine(re)
	, m_input(input)
	, m_clipboard(clipboard)
	, m_fs(fs)
	, _texman(re->GetTextureManager())
	, _tsCurrent(_timestep.end())
	, _tsDeleteCurrent(false)
	, _captureCountSystem(0)
	, _focusWnd(nullptr)
	, _hotTrackWnd(nullptr)
	, _desktop(nullptr)
	, _isAppActive(false)
#ifndef NDEBUG
	, _dbgFocusIsChanging(false)
#endif
{
	m_renderingEngine->GetScheme().RegisterDrawable(*this);

	m_input->AddListener(this);
	m_renderingEngine->GetWindow()->AddListener(this);
}

LayoutManager::~LayoutManager()
{
	_desktop->Destroy();

	m_input->RemoveListener(this);
	m_renderingEngine->GetWindow()->RemoveListener(this);

	m_renderingEngine->GetScheme().UnegisterDrawable(*this);
}

int LayoutManager::GetOrder() const
{
	return static_cast<int>(RenderOrder::GUI);
}

UIWindow* LayoutManager::GetCapture(unsigned int pointerID) const
{
	auto it = _pointerCaptures.find(pointerID);
	return _pointerCaptures.end() != it ? it->second.captureWnd.Get() : nullptr;
}

bool LayoutManager::HasCapturedPointers(UIWindow *wnd) const
{
	for (auto &capture: _pointerCaptures)
	{
		if (capture.second.captureWnd.Get() == wnd)
		{
			return true;
		}
	}
	return false;
}

UIWindow* LayoutManager::GetDesktop() const
{
	return _desktop.Get();
}

void LayoutManager::SetCapture(unsigned int pointerID, UIWindow *wnd)
{
	if( wnd )
	{
		if( _pointerCaptures[pointerID].captureWnd.Get() )
		{
			assert(_pointerCaptures[pointerID].captureWnd.Get() == wnd);
			assert(_pointerCaptures[pointerID].captureCount != 0);
		}
		else
		{
			//assert(0 == _pointerCaptures[pointerID].captureCount);
			_pointerCaptures[pointerID].captureWnd.Set(wnd);
		}
		_pointerCaptures[pointerID].captureCount++;
	}
	else
	{
		//assert(_pointerCaptures[pointerID].captureWnd.Get());
		//assert(0 != _pointerCaptures[pointerID].captureCount);
		if( 0 == --_pointerCaptures[pointerID].captureCount )
		{
			_pointerCaptures[pointerID].captureWnd.Set(nullptr);
		}
	}
}

bool LayoutManager::SetFocusWnd(UIWindow* wnd)
{
	assert(!_dbgFocusIsChanging);
	if( _focusWnd.Get() != wnd )
	{
		WindowWeakPtr wp(wnd);
		WindowWeakPtr oldFocusWnd(_focusWnd.Get());

		// try setting new focus. it should not change _focusWnd
#ifndef NDEBUG
		_dbgFocusIsChanging = true;
#endif
		bool focusAccepted = wnd && wnd->GetEnabledCombined() && wnd->GetVisibleCombined() && wnd->OnFocus(true);
#ifndef NDEBUG
		_dbgFocusIsChanging = false;
#endif
		if( !focusAccepted && wp.Get() && oldFocusWnd.Get() )
		{
			for(UIWindow *w = wp.Get()->GetParent(); w; w = w->GetParent() )
			{
				if( w == oldFocusWnd.Get() )
				{
					// don't reset focus from parent
					return false;
				}
			}
		}

		// set new focus
		_focusWnd.Set(focusAccepted ? wp.Get() : nullptr);
		assert(!_focusWnd.Get() || _focusWnd->GetEnabledCombined() && _focusWnd->GetVisibleCombined());

		// reset old focus
		if( oldFocusWnd.Get() && oldFocusWnd.Get() != _focusWnd.Get() )
		{
			oldFocusWnd->OnFocus(false); // _focusWnd may be destroyed here
			if( oldFocusWnd.Get() && oldFocusWnd->eventLostFocus )
				oldFocusWnd->eventLostFocus();
		}
	}
	return nullptr != _focusWnd.Get();
}

UIWindow* LayoutManager::GetFocusWnd() const
{
	assert(!_dbgFocusIsChanging);
	return _focusWnd.Get();
}

/*
bool LayoutManager::ResetFocus(Window* wnd)
{
	assert(wnd);
	assert(_focusWnd);

	if( wnd == _focusWnd )
	{
		//
		// search for first appropriate parent
		//

		Window *tmp = wnd;
		for( Window *w = wnd->GetParent(); w; w = w->GetParent() )
		{
			if( !w->GetVisibleCombined() || !w->GetEnabled() )
			{
				tmp = w->GetParent();
			}
		}

		while( tmp )
		{
			if( wnd != tmp && SetFocusWnd(tmp) )
			{
				break;
			}

			Window *r;

			// try to pass focus to next siblings
			for( r = tmp->GetNextSibling(); r; r = r->GetNextSibling() )
			{
				if( r->GetVisibleCombined() && r->GetEnabled() )
				{
					if( SetFocusWnd(r) ) break;
				}
			}
			if( r ) break;

			// try to pass focus to previous siblings
			for( r = tmp->GetPrevSibling(); r; r = r->GetPrevSibling() )
			{
				if( r->GetVisibleCombined() && r->GetEnabled() )
				{
					if( SetFocusWnd(r) ) break;
				}
			}
			if( r ) break;

			// and finally try to pass focus to the parent and its siblings
			tmp = tmp->GetParent();
			assert(!tmp || (tmp->GetVisibleCombined() && tmp->GetEnabled()));
		}
		if( !tmp )
		{
			SetFocusWnd(nullptr);
		}
		assert(wnd != _focusWnd);
		return true;
	}

	for( Window *w = wnd->GetFirstChild(); w; w = w->GetNextSibling() )
	{
		if( ResetFocus(w) )
		{
			return true;
		}
	}

	return false;
}
*/
void LayoutManager::ResetWindow(UIWindow* wnd)
{
	assert(wnd);

	if( GetFocusWnd() == wnd )
		SetFocusWnd(nullptr);

	if( _hotTrackWnd.Get() == wnd )
	{
		_hotTrackWnd->OnMouseLeave();
		_hotTrackWnd.Set(nullptr);
	}

	_pointerCaptures.clear();
}

std::list<UIWindow*>::iterator LayoutManager::TimeStepRegister(UIWindow* wnd)
{
	_timestep.push_front(wnd);
	return _timestep.begin();
}

void LayoutManager::TimeStepUnregister(std::list<UIWindow*>::iterator it)
{
    if( _tsCurrent == it )
    {
        assert(!_tsDeleteCurrent);
        _tsDeleteCurrent = true;
    }
    else
    {
        _timestep.erase(it);
    }
}

void LayoutManager::Update(float realDeltaTime)
{
    assert(_tsCurrent == _timestep.end());
    assert(!_tsDeleteCurrent);
	for( _tsCurrent = _timestep.begin(); _tsCurrent != _timestep.end(); )
	{
		(*_tsCurrent)->OnTimeStep(realDeltaTime);
        if (_tsDeleteCurrent)
        {
            _tsDeleteCurrent = false;
            _tsCurrent = _timestep.erase(_tsCurrent);
        }
        else
        {
            ++_tsCurrent;
        }
	}
}

bool LayoutManager::ProcessPointerInternal(UIWindow* wnd, float x, float y, float z, Msg msg, int buttons, PointerType pointerType, unsigned int pointerID, bool topMostPass, bool insideTopMost)
{
    insideTopMost |= wnd->GetTopMost();
    
    if (!wnd->GetEnabled() || !wnd->GetVisible() || (insideTopMost && !topMostPass))
        return false;
    
	bool pointerInside = (x >= 0 && x < wnd->GetWidth() && y >= 0 && y < wnd->GetHeight());

	if( (pointerInside || !wnd->GetClipChildren()) && GetCapture(pointerID) != wnd )
	{
        // route message to each child in reverse order until someone process it
        for(UIWindow *w = wnd->GetLastChild(); w; w = w->GetPrevSibling() )
        {
#ifndef NDEBUG
            WindowWeakPtr wp(w);
#endif
            if( ProcessPointerInternal(w, x - w->GetX(), y - w->GetY(), z, msg, buttons, pointerType, pointerID, topMostPass, insideTopMost) )
            {
                return true;
            }
            assert(wp.Get());
        }
	}

	if( insideTopMost == topMostPass && (pointerInside || GetCapture(pointerID) == wnd) )
	{
		// window is captured or the pointer is inside the window

		WindowWeakPtr wp(wnd);

		bool msgProcessed = false;
		switch( msg )
		{
			case Msg::PointerDown:
				msgProcessed = wnd->OnPointerDown(x, y, buttons, pointerType, pointerID);
				break;
			case Msg::PointerUp:
			case Msg::PointerCancel:
				msgProcessed = wnd->OnPointerUp(x, y, buttons, pointerType, pointerID);
				break;
			case Msg::PointerMove:
				msgProcessed = wnd->OnPointerMove(x, y, pointerType, pointerID);
				break;
			case Msg::MOUSEWHEEL:
				msgProcessed = wnd->OnMouseWheel(x, y, z);
				break;
			case Msg::TAP:
				msgProcessed = wnd->OnTap(x, y);
				break;
			default:
				assert(false);
		}
		// if window did not process the message, it should not destroy itself
		assert(msgProcessed || wp.Get());

		if( wp.Get() && msgProcessed )
		{
			switch( msg )
			{
			case Msg::PointerDown:
            case Msg::TAP:
				SetFocusWnd(wnd); // may destroy wnd
            default:
                break;
			}

			if( wp.Get() && wnd != _hotTrackWnd.Get() )
			{
				if( _hotTrackWnd.Get() )
					_hotTrackWnd->OnMouseLeave(); // may destroy wnd
				if( wp.Get() && wnd->GetVisibleCombined() && wnd->GetEnabledCombined() )
				{
					_hotTrackWnd.Set(wnd);
					_hotTrackWnd->OnMouseEnter(x, y);
				}
			}
		}

		return msgProcessed;
	}

	return false;
}

void LayoutManager::OnMouseButtonDown(MouseButton button)
{
	UI::Msg msg = UI::Msg::PointerDown;
	int buttons = 0;

	switch (button)
	{
	case MouseButton::Left:
		buttons |= 0x01;
		break;

	case MouseButton::Right:
		buttons |= 0x02;
		break;

	case MouseButton::Middle:
		buttons |= 0x04;
		break;

	default: ;
	}

	const Vector2& pos = m_input->GetMousePosition();
	ProcessPointer(pos.x, pos.y, 0, Msg::PointerDown, buttons, PointerType::Mouse, 0);
}

void LayoutManager::OnMouseButtonUp(MouseButton button)
{
	Msg msg = Msg::PointerDown;
	int buttons = 0;

	switch (button)
	{
	case MouseButton::Left:
		buttons |= 0x01;
		break;

	case MouseButton::Right:
		buttons |= 0x02;
		break;

	case MouseButton::Middle:
		buttons |= 0x04;
		break;

	default:;
	}

	const Vector2& pos = m_input->GetMousePosition();
	ProcessPointer(pos.x, pos.y, 0, Msg::PointerUp, buttons, PointerType::Mouse, 0);
}

void LayoutManager::OnMousePosition(float x, float y)
{
	ProcessPointer(x, y, 0, UI::Msg::PointerMove, 0, UI::PointerType::Mouse, 0);
}

void LayoutManager::OnMouseScrollOffset(float x, float y)
{
	const Vector2& pos = m_input->GetMousePosition();
	ProcessPointer(pos.x, pos.y, y, Msg::MOUSEWHEEL, 0, PointerType::Mouse, 0);

	std::cout << "OnMouseScrollOffset" << std::endl;
}

void LayoutManager::OnKeyDown(Key key)
{
	ProcessKeys(Msg::KEYDOWN, key);

	std::cout << "OnKeyDown" << std::endl;
}

void LayoutManager::OnKey(Key key)
{
}

void LayoutManager::OnCharacter(char character)
{
	ProcessText(character);

	std::cout << "OnCharacter " << character << std::endl;
}

void LayoutManager::OnKeyUp(Key key)
{
	ProcessKeys(Msg::KEYUP, key);

	std::cout << "OnKeyUp" << std::endl;
}

void LayoutManager::OnFrameBufferChanged(int width, int height)
{
	_desktop->Resize(width, height);
}

void LayoutManager::OnSizeChanged(int width, int height)
{
	std::cout << "OnSizeChanged" << std::endl;
}

void LayoutManager::OnClosed()
{
	std::cout << "OnClosed" << std::endl;
}

bool LayoutManager::ProcessPointer(float x, float y, float z, Msg msg, int button, PointerType pointerType, unsigned int pointerID)
{
#ifndef NDEBUG
    _lastPointerLocation[pointerID] = Vector2(x, y);
#endif

	if(UIWindow *captured = GetCapture(pointerID) )
	{
		// calc relative mouse position and route message to captured window
		for(UIWindow *wnd = captured; _desktop.Get() != wnd; wnd = wnd->GetParent() )
		{
			assert(wnd);
			x -= wnd->GetX();
			y -= wnd->GetY();
		}
		if( ProcessPointerInternal(captured, x, y, z, msg, button, pointerType, pointerID, true) ||
            ProcessPointerInternal(captured, x, y, z, msg, button, pointerType, pointerID, false))
			return true;
	}
	else
	{
#ifndef NDEBUG
        if (!_desktop.Get())
            return false;
#else
        assert(_desktop.Get());
#endif
        
		// handle all children of the desktop recursively; offer to topmost windows first
		if( ProcessPointerInternal(_desktop.Get(), x, y, z, msg, button, pointerType, pointerID, true) ||
            ProcessPointerInternal(_desktop.Get(), x, y, z, msg, button, pointerType, pointerID, false))
			return true;
	}
	if( _hotTrackWnd.Get() )
	{
		_hotTrackWnd->OnMouseLeave();
		_hotTrackWnd.Set(nullptr);
	}
	return false;
}

bool LayoutManager::ProcessKeys(Msg msg, Key key)
{
	switch( msg )
	{
	case Msg::KEYUP:
		break;
	case Msg::KEYDOWN:
		if(UIWindow *wnd = GetFocusWnd() )
		{
			while( wnd )
			{
				if( wnd->OnKeyPressed(key) )
				{
					return true;
				}
				wnd = wnd->GetParent();
			}
		}
		else
		{
			GetDesktop()->OnKeyPressed(key);
		}
		break;
	default:
		assert(false);
	}

	return false;
}

bool LayoutManager::ProcessText(int c) const
{
	if (UIWindow *wnd = GetFocusWnd())
	{
		while (wnd)
		{
			if (wnd->OnChar(c))
			{
				return true;
			}
			wnd = wnd->GetParent();
		}
	}
	else
	{
		GetDesktop()->OnChar(c);
	}
	return false;
}

static void DrawWindowRecursive(const UIWindow &wnd, DrawingContext &dc, bool topMostPass, bool insideTopMost)
{
    insideTopMost |= wnd.GetTopMost();

    if (!wnd.GetVisible() || (insideTopMost && !topMostPass))
        return; // skip window and all its children
    
	dc.PushTransform(Vector2(wnd.GetX(), wnd.GetY()));

    if (insideTopMost == topMostPass)
        wnd.Draw(dc);
    
    // topmost windows escape parents' clip
    bool clipChildren = wnd.GetClipChildren() && (!topMostPass || insideTopMost);

	if (clipChildren)
	{
		math::RectInt clip;
		clip.left = 0;
		clip.top = 0;
		clip.right = (int)wnd.GetWidth();
		clip.bottom = (int)wnd.GetHeight();
		dc.PushClippingRect(clip);
	}

	for (UIWindow *w = wnd.GetFirstChild(); w; w = w->GetNextSibling())
        DrawWindowRecursive(*w, dc, topMostPass, wnd.GetTopMost() || insideTopMost);

	if (clipChildren)
		dc.PopClippingRect();

	dc.PopTransform();
}

void LayoutManager::Draw(DrawingContext &dc, float interpolation) const
{
	dc.SetMode(INTERFACE);

    DrawWindowRecursive(*_desktop.Get(), dc, false, false);
    DrawWindowRecursive(*_desktop.Get(), dc, true, false);

#ifndef NDEBUG
	for (auto &id2pos: _lastPointerLocation)
	{
		math::RectFloat dst = { id2pos.second.x-4, id2pos.second.y-4, id2pos.second.x+4, id2pos.second.y+4 };
		dc.DrawSprite(&dst, 0U, 0xffffffff, 0U);
	}
#endif
}

} // namespace UI
