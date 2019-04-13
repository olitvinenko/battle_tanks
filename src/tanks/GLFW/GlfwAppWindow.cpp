#include "GlfwAppWindow.h"
#include "GlfwKeys.h"
#include "GuiManager.h"
#include "Pointers.h"
#include "UIWindow.h"
#include "Rendering/RenderOpenGL.h"
#include "GlfwInput.h"
#include "GlfwClipboard.h"

namespace GLFWProcessors
{
	void OnMouseButton(GLFWwindow *window, int button, int action, int mods)
	{
		if (auto gui = GlfwAppWindow::GetLayoutManager(window))
		{
			UI::Msg msg = (GLFW_RELEASE == action) ? UI::Msg::PointerUp : UI::Msg::PointerDown;
			int buttons = 0;
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				buttons |= 0x01;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				buttons |= 0x02;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				buttons |= 0x04;
				break;
			default:
				return;
			}

			//Vector2 mousePos = GetCursorPosInPixels(window);
			//gui->ProcessPointer(mousePos.x, mousePos.y, 0, msg, buttons, UI::PointerType::Mouse, 0);
		}
	}

	void OnCursorPos(GLFWwindow *window, double xpos, double ypos)
	{
		if (auto gui = GlfwAppWindow::GetLayoutManager(window))
		{
			//Vector2 mousePos = GetCursorPosInPixels(window, xpos, ypos);
			//gui->ProcessPointer(mousePos.x, mousePos.y, 0, UI::Msg::PointerMove, 0, UI::PointerType::Mouse, 0);
		}
	}

	void OnScroll(GLFWwindow *window, double xoffset, double yoffset)
	{
		if (auto gui = GlfwAppWindow::GetLayoutManager(window))
		{
			//Vector2 mousePos = GetCursorPosInPixels(window);
			//Vector2 mouseOffset = GetCursorPosInPixels(window, xoffset, yoffset);

			//gui->ProcessPointer(mousePos.x, mousePos.y, mouseOffset.y, UI::Msg::MOUSEWHEEL, 0, UI::PointerType::Mouse, 0);
		}
	}

	void OnKey(GLFWwindow *window, int platformKey, int scancode, int action, int mods)
	{
		if (auto gui = GlfwAppWindow::GetLayoutManager(window))
		{
			//Key key = MapGlfwKeyCode(platformKey);
			//gui->ProcessKeys(GLFW_RELEASE == action ? UI::Msg::KEYUP : UI::Msg::KEYDOWN, key);
		}
	}

	void OnChar(GLFWwindow *window, unsigned int codepoint)
	{
		if (auto gui = GlfwAppWindow::GetLayoutManager(window))
		{
			if (codepoint < 57344 || codepoint > 63743) // ignore Private Use Area characters
			{
				//gui->ProcessText(codepoint);
			}
		}
	}

	void OnFramebufferSize(GLFWwindow *window, int width, int height)
	{
		if (auto gui = GlfwAppWindow::GetLayoutManager(window))
		{
			gui->GetDesktop()->Resize((float)width, (float)height);
		}
	}
}


GlfwAppWindow::GlfwAppWindow(const char *title, bool fullscreen, int width, int height)
{
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize OpenGL");
	}

	m_window = (glfwCreateWindow(
		fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->width : width,
		fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->height : height,
		title,
		fullscreen ? glfwGetPrimaryMonitor() : nullptr,
		nullptr));

	//m_clipboard.reset(new GlfwClipboard(*m_window));
	//m_input.reset(new GlfwInput(*m_window));
	m_render.reset(new RenderOpenGL());

	glfwSetMouseButtonCallback(m_window, GLFWProcessors::OnMouseButton);
	glfwSetCursorPosCallback(m_window, GLFWProcessors::OnCursorPos);
	glfwSetScrollCallback(m_window, GLFWProcessors::OnScroll);
	glfwSetKeyCallback(m_window, GLFWProcessors::OnKey);
	glfwSetCharCallback(m_window, GLFWProcessors::OnChar);
	glfwSetFramebufferSizeCallback(m_window, GLFWProcessors::OnFramebufferSize);


	glfwSetWindowSizeCallback(m_window, GLFWProcessors::OnFramebufferSize);
	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* w) {});

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);	
}

GlfwAppWindow::~GlfwAppWindow()
{
	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

IClipboard& GlfwAppWindow::GetClipboard()
{
	return *m_clipboard;
}

IInput& GlfwAppWindow::GetInput()
{
	return *m_input;
}

IRender& GlfwAppWindow::GetRender()
{
	return *m_render;
}

unsigned int GlfwAppWindow::GetPixelWidth()
{
	int width;
	glfwGetFramebufferSize(m_window, &width, nullptr);
	return width;
}

unsigned int GlfwAppWindow::GetPixelHeight()
{
	int height;
	glfwGetFramebufferSize(m_window, nullptr, &height);
	return height;
}

void GlfwAppWindow::SetUserPointer(UI::LayoutManager *inputSink)
{
	glfwSetWindowUserPointer(m_window, inputSink);
}

UI::LayoutManager* GlfwAppWindow::GetLayoutManager(GLFWwindow * window) // static
{
	return static_cast<UI::LayoutManager *>(glfwGetWindowUserPointer(window));
}

void GlfwAppWindow::SwapBuffers() const
{
	glfwSwapBuffers(m_window);
}

bool GlfwAppWindow::ShouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

void GlfwAppWindow::PollEvents() const
{
	glfwPollEvents();
}
