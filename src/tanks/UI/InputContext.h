#pragma once
#include "Pointers.h"
#include "math/MyMath.h"
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

class TextureManager;

namespace UI
{

enum class Key;
struct IInput;
struct IClipboard;
struct PointerSink;
class LayoutContext;
class StateContext;
class Window;

enum class Msg
{
	KEYUP,
	KEYDOWN,
	PointerDown,
	PointerMove,
	PointerUp,
	PointerCancel,
	Scroll,
	TAP,
};

class InputContext
{
public:
	InputContext(IInput &input, IClipboard &clipboard);

	bool ProcessPointer(
		TextureManager &texman,
		std::shared_ptr<Window> wnd,
		const LayoutContext &lc,
		const StateContext &sc,
		vec2d pxPointerPosition,
		vec2d pxPointerOffset,
		Msg msg,
		int button,
		PointerType pointerType,
		unsigned int pointerID);
	bool ProcessKeys(TextureManager &texman, std::shared_ptr<Window> wnd, const LayoutContext &lc, const StateContext &sc, Msg msg, UI::Key key);
	bool ProcessText(TextureManager &texman, std::shared_ptr<Window> wnd, const LayoutContext &lc, const StateContext &sc, int c);

	IClipboard &GetClipboard() const { return _clipboard; }
	IInput& GetInput() const { return _input; }

	void PushTransform(vec2d offset, bool focused, bool hovered);
	void PopTransform();

	vec2d GetMousePos() const;
	bool GetFocused() const;
	bool GetHovered() const;

	const std::vector<std::shared_ptr<Window>>* GetCapturePath(unsigned int pointerID) const;
	bool HasCapturedPointers(const Window* wnd) const;

	bool GetMainWindowActive() const { return _isAppActive; }

#ifndef NDEBUG
	const std::unordered_map<unsigned int, vec2d>& GetLastPointerLocation() const { return _lastPointerLocation; }
#endif

private:
	bool ProcessScroll(TextureManager &texman, std::shared_ptr<Window> wnd, const LayoutContext &lc, const StateContext &sc, vec2d pxPointerPosition, vec2d offset);

	IInput &_input;
	IClipboard &_clipboard;

	struct InputStackFrame
	{
		vec2d offset;
		bool focused;
		bool hovered;
	};
	std::stack<InputStackFrame> _transformStack;

	struct PointerCapture
	{
		std::vector<std::shared_ptr<Window>> capturePath;
	};

	std::unordered_map<unsigned int, PointerCapture> _pointerCaptures;

	bool _isAppActive;
#ifndef NDEBUG
	std::unordered_map<unsigned int, vec2d> _lastPointerLocation;
#endif
};

class LayoutContext;

struct AreaSinkSearch
{
	TextureManager &texman;
	vec2d pxGlobalPointerPosition;
	bool topMostPass;
	std::vector<std::shared_ptr<Window>> outSinkPath;
};

template<class SinkType>
SinkType* FindAreaSink(
	AreaSinkSearch &search,
	std::shared_ptr<Window> wnd,
	const LayoutContext &lc,
	const StateContext &sc,
	const InputContext &ic,
	bool insideTopMost);

} // namespace UI
