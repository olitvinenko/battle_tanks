#pragma once
#include "ui/AppWindow.h"
#include "common/NonCopyable.h"
#include <memory>

class GlfwClipboard;
class GlfwInput;
struct GLFWwindow;

struct GlfwWindowDeleter
{
	void operator()(GLFWwindow *window);
};

class GlfwAppWindow
    : public AppWindow
    , private NonCopyable
{
public:
	GlfwAppWindow(const char *title, bool fullscreen, int width, int height);
	~GlfwAppWindow();

	static void PollEvents();
	void Present();
	bool ShouldClose() const;

	float GetPixelWidth() const;
	float GetPixelHeight() const;
	float GetLayoutScale() const;

	// AppWindow
	UI::IClipboard& GetClipboard() override;
	UI::IInput& GetInput() override;
	IRender& GetRender() override;
	void SetInputSink(UI::LayoutManager *inputSink) override;
	void MakeCurrent() override;

private:
	std::unique_ptr<GLFWwindow, GlfwWindowDeleter> _window;
	std::unique_ptr<GlfwClipboard> _clipboard;
	std::unique_ptr<GlfwInput> _input;
	std::unique_ptr<IRender> _render;
};
