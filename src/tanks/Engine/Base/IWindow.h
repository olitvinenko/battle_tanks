#pragma once

#include <string>

struct IWindow
{
	virtual ~IWindow() = default;

	virtual int GetPixelWidth() const = 0;
	virtual int GetPixelHeight() const = 0;
	virtual float GetAspectRatio() const = 0;

	virtual bool ShouldClose() const = 0;
	virtual void SwapBuffers() = 0;
	virtual void PollEvents() = 0;

	virtual const std::string& GetName() const = 0;
};
