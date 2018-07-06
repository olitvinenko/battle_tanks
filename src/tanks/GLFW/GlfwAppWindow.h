#pragma once
#include <memory>
#include "AppWindowBase.h"

class GlfwClipboard;
class GlfwInput;
struct GLFWwindow;

struct IRender;

class GlfwAppWindow : public AppWindowBase
{
public:
	GlfwAppWindow(const char *title, bool fullscreen, int width, int height);
	~GlfwAppWindow();

	void PollEvents() const;
	static UI::LayoutManager* GetLayoutManager(GLFWwindow * window);

	void SwapBuffers() const;
	bool ShouldClose() const;

	// AppWindow
	UI::IClipboard& GetClipboard() override;
	UI::IInput& GetInput() override;
	IRender& GetRender() override;
	unsigned int GetPixelWidth() override;
	unsigned int GetPixelHeight() override;
	void SetUserPointer(UI::LayoutManager *inputSink) override;

private:
	struct GlfwWindowDeleter
	{
		void operator()(GLFWwindow *window) const;
	};

	std::unique_ptr<GLFWwindow, GlfwWindowDeleter> m_window;
	std::unique_ptr<GlfwClipboard> m_clipboard;
	std::unique_ptr<GlfwInput> m_input;
	std::unique_ptr<IRender> m_render;
};