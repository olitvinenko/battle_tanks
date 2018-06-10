#pragma once

struct IRender;

namespace UI
{
	struct IInput;
	struct IClipboard;
	class LayoutManager;
}

struct AppWindow
{
	virtual UI::IClipboard& GetClipboard() = 0;
	virtual UI::IInput& GetInput() = 0;
	virtual IRender& GetRender() = 0;
	virtual unsigned int GetPixelWidth() = 0;
	virtual unsigned int GetPixelHeight() = 0;
	virtual void SetUserPointer(UI::LayoutManager *inputSink) = 0;

	virtual ~AppWindow() = default;
};
