#pragma once
#include "WindowPtr.h"
#include "Pointers.h"
#include <list>
#include <unordered_map>
#include "Vector2.h"


class DrawingContext;
class TextureManager;

namespace UI
{

class UIWindow;
class LayoutManager;
enum class Key;
struct IInput;
struct IClipboard;

struct IWindowFactory
{
	virtual UIWindow* Create(LayoutManager *pManager) = 0;
	virtual ~IWindowFactory() = default;
};

enum class Msg
{
    KEYUP,
    KEYDOWN,
    PointerDown,
    PointerMove,
    PointerUp,
    PointerCancel,
	MOUSEWHEEL,
    TAP,
};

class LayoutManager
{
public:
	LayoutManager(IInput &input, IClipboard &clipboard, TextureManager &texman, IWindowFactory &&desktopFactory);
	~LayoutManager();

	void TimeStep(float dt);
	void Render(DrawingContext &dc) const;

	bool ProcessPointer(float x, float y, float z, Msg msg, int button, PointerType pointerType, unsigned int pointerID);
	bool ProcessKeys(Msg msg, UI::Key key);
	bool ProcessText(int c) const;

	IClipboard &GetClipboard() const { return _clipboard; }
	IInput& GetInput() const { return _input; }
	TextureManager& GetTextureManager() const { return _texman; }
	UIWindow* GetDesktop() const;

	UIWindow* GetCapture(unsigned int pointerID) const;
	void SetCapture(unsigned int pointerID, UIWindow* wnd);
    bool HasCapturedPointers(UIWindow* wnd) const;

	bool SetFocusWnd(UIWindow* wnd);  // always resets previous focus
	UIWindow* GetFocusWnd() const;
//	bool ResetFocus(Window* wnd);   // remove focus from wnd or any of its children

	bool IsMainWindowActive() const { return _isAppActive; }

private:
	friend class UIWindow;
	void ResetWindow(UIWindow* wnd);
    std::list<UIWindow*>::iterator TimeStepRegister(UIWindow* wnd);
	void TimeStepUnregister(std::list<UIWindow*>::iterator it);

	bool ProcessPointerInternal(UIWindow* wnd, float x, float y, float z, Msg msg, int buttons, PointerType pointerType, unsigned int pointerID, bool topMostPass, bool insideTopMost = false);

	IInput &_input;
	IClipboard &_clipboard;
    TextureManager &_texman;
    std::list<UIWindow*> _timestep;
    std::list<UIWindow*>::iterator _tsCurrent;
    bool _tsDeleteCurrent;
    
    struct PointerCapture
    {
        unsigned int captureCount = 0;
        WindowWeakPtr captureWnd;
    };
    
    std::unordered_map<unsigned int, PointerCapture> _pointerCaptures;


	unsigned int _captureCountSystem;

	WindowWeakPtr _focusWnd;
	WindowWeakPtr _hotTrackWnd;

	WindowWeakPtr _desktop;

	bool _isAppActive;
#ifndef NDEBUG
	bool _dbgFocusIsChanging;
    std::unordered_map<unsigned int, math::Vector2> _lastPointerLocation;
#endif
};

} // namespace UI
