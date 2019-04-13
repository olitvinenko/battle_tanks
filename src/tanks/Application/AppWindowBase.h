#pragma once

struct IRender;
struct IInput;
struct IClipboard;

namespace UI
{
	class LayoutManager;
}

struct AppWindowBase
{
	virtual IClipboard& GetClipboard() = 0;
	virtual IInput& GetInput() = 0;
	virtual IRender& GetRender() = 0;
	virtual unsigned int GetPixelWidth() = 0;
	virtual unsigned int GetPixelHeight() = 0;
	virtual void SetUserPointer(UI::LayoutManager *inputSink) = 0;

	virtual ~AppWindowBase() = default;
};
