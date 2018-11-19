#include "GlfwWindow.h"
#include "GLFW/glfw3.h"
#include <functional>

GlfwWindow* GlfwWindow::m_instance = nullptr;

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

	m_instance = this;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(m_window, OnFramebufferSizeCallback);
	glfwSetWindowSizeCallback(m_window, OnSizeCallback);
	glfwSetWindowCloseCallback(m_window, OnCloseCallback);
}

GlfwWindow::~GlfwWindow()
{
	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();

	m_instance = nullptr;
}

int GlfwWindow::GetPixelWidth() const
{
	int width;
	glfwGetFramebufferSize(m_window, &width, nullptr);
	return width;
}

void GlfwWindow::AddListener(IWindowListener* listener)
{
	m_listeners.insert(listener);
}

void GlfwWindow::RemoveListener(IWindowListener* listener)
{
	m_listeners.erase(listener);
}

void GlfwWindow::OnFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	for (auto listener : m_instance->m_listeners)
		listener->OnFrameBufferChanged(width, height);
}

void GlfwWindow::OnSizeCallback(GLFWwindow *window, int width, int height)
{
	for (auto listener : m_instance->m_listeners)
		listener->OnSizeChanged(width, height);
}

void GlfwWindow::OnCloseCallback(GLFWwindow *window)
{
	for (auto listener : m_instance->m_listeners)
		listener->OnClosed();
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