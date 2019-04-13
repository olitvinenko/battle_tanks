#pragma once
#include "WindowPtr.h"
#include "Pointers.h"
#include <list>
#include <unordered_map>
#include "Vector2.h"
#include "Base/IWindow.h"
#include "Base/IInput.h"
#include "Rendering/IDrawable.h"
#include "Rendering/RenderingEngine.h"


class DrawingContext;
class TextureManager;
enum class Key;
class AppController;

struct IInput;
struct IClipboard;

namespace UI
{

class UIWindow;
class LayoutManager;

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

class LayoutManager final
	: public IDrawable
	, IInputListener
	, IWindowListener
{
public:
	//TODO:: delete
	LayoutManager(IInput &input, IClipboard &clipboard, TextureManager &texman, AppController& desktopFactory);


	LayoutManager(std::shared_ptr<RenderingEngine> re, std::shared_ptr<FileSystem::IFileSystem> fs, std::shared_ptr<IInput> input, std::shared_ptr<IClipboard> clipboard);

	//IDrawable
	int GetOrder() const override;
	void Draw(DrawingContext& dc, float interpolation) const override;

	void Initialize();
	
	void Update(float realDeltaTime);
	
	~LayoutManager();

	IClipboard& GetClipboard() const { return *m_clipboard; }
	IInput& GetInput() const { return *m_input; }
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
	bool ProcessPointer(float x, float y, float z, Msg msg, int button, PointerType pointerType, unsigned int pointerID);
	bool ProcessKeys(Msg msg, Key key);
	bool ProcessText(int c) const;

	//TODO::
	//> IInputListener
	void OnMouseButtonDown(MouseButton button) override;
	void OnMouseButtonUp(MouseButton button) override;
	void OnMousePosition(float x, float y) override;
	void OnMouseScrollOffset(float x, float y) override;
	void OnCharacter(char character) override;
	void OnKeyDown(Key key) override;
	void OnKeyUp(Key key) override;
	void OnKey(Key key) override;

	//-> IWindowListener
	void OnFrameBufferChanged(int width, int height) override;
	void OnSizeChanged(int width, int height) override;
	void OnClosed() override;


	friend class UIWindow;
	void ResetWindow(UIWindow* wnd);
    std::list<UIWindow*>::iterator TimeStepRegister(UIWindow* wnd);
	void TimeStepUnregister(std::list<UIWindow*>::iterator it);

	bool ProcessPointerInternal(UIWindow* wnd, float x, float y, float z, Msg msg, int buttons, PointerType pointerType, unsigned int pointerID, bool topMostPass, bool insideTopMost = false);

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
    std::unordered_map<unsigned int, Vector2> _lastPointerLocation;
#endif

	std::shared_ptr<RenderingEngine> m_renderingEngine;
	std::shared_ptr<IInput> m_input;
	std::shared_ptr<IClipboard> m_clipboard;
	std::shared_ptr<FileSystem::IFileSystem>  m_fs;
};

} // namespace UI
