#include "GlfwWindow.h"
#include "GLFW/glfw3.h"

GlfwWindow::GlfwWindow(const char* title, int width, int height, bool fullscreen)
	: m_name(title)
{
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize OpenGL");
	}

	m_window = glfwCreateWindow(
		fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->width : width,
		fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->height : height,
		title,
		fullscreen ? glfwGetPrimaryMonitor() : nullptr,
		nullptr);

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
}

GlfwWindow::~GlfwWindow()
{
	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

int GlfwWindow::GetPixelWidth() const
{
	int width;
	glfwGetFramebufferSize(m_window, &width, nullptr);
	return width;
}

int GlfwWindow::GetPixelHeight() const
{
	int height;
	glfwGetFramebufferSize(m_window, nullptr, &height);
	return height;
}

float GlfwWindow::GetAspectRatio() const
{
	return (float)GetPixelWidth() / (float)GetPixelHeight();
}

bool GlfwWindow::ShouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

void GlfwWindow::SwapBuffers()
{
	glfwSwapBuffers(m_window);
}

void GlfwWindow::PollEvents()
{
	glfwPollEvents();
}