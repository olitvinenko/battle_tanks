#pragma once
#include <memory>

class GlfwClipboard;
class GlfwInput;
struct GLFWwindow;

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

class GlfwAppWindow : public AppWindow
{
public:
	GlfwAppWindow(const char *title, bool fullscreen, int width, int height);
	~GlfwAppWindow();

	void PollEvents() const;
	static UI::LayoutManager* GetLayoutManager(GLFWwindow * window);

	void Present() const;
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