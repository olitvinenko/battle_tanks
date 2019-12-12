//#include "GlfwWindow.h"
//#include "GLFW/glfw3.h"
//#include "math/Vector2.h"
//#include <functional>
//
//GlfwWindow* GlfwWindow::m_instance = nullptr;
//
//GlfwWindow::GlfwWindow(const char* title, int width, int height, bool fullscreen)
//	: m_name(title)
//{
//	if (!glfwInit())
//	{
//		throw std::runtime_error("Failed to initialize OpenGL");
//	}
//
//	m_glfwWindow = glfwCreateWindow(
//		fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->width : width,
//		fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->height : height,
//		title,
//		fullscreen ? glfwGetPrimaryMonitor() : nullptr,
//		nullptr);
//
//	m_instance = this;
//
//	glfwMakeContextCurrent(m_glfwWindow);
//	glfwSwapInterval(1);
//
//	glfwSetFramebufferSizeCallback(m_glfwWindow, OnFramebufferSizeCallback);
//	glfwSetWindowSizeCallback(m_glfwWindow, OnSizeCallback);
//	glfwSetWindowCloseCallback(m_glfwWindow, OnCloseCallback);
//}
//
//GlfwWindow::~GlfwWindow()
//{
//	glfwMakeContextCurrent(nullptr);
//	glfwDestroyWindow(m_glfwWindow);
//	glfwTerminate();
//
//	m_instance = nullptr;
//}
//
//Vector2 GlfwWindow::GetCursorPosInPixels() const
//{
//    double dipX = 0;
//    double dipY = 0;
//    glfwGetCursorPos(m_glfwWindow, &dipX, &dipY);
//    return GetCursorPosInPixels(dipX, dipY);
//}
//
//Vector2 GlfwWindow::GetCursorPosInPixels(double dipX, double dipY) const
//{
//    int pxWidth;
//    int pxHeight;
//    glfwGetFramebufferSize(m_glfwWindow, &pxWidth, &pxHeight);
//
//    int dipWidth;
//    int dipHeight;
//    glfwGetWindowSize(m_glfwWindow, &dipWidth, &dipHeight);
//
//    return{ float(dipX * pxWidth / dipWidth), float(dipY * pxHeight / dipHeight) };
//}
//
//Vector2 GlfwWindow::GetPixelSize() const
//{
//    int width;
//    int height;
//    glfwGetFramebufferSize(m_glfwWindow, &width, &height);
//    return Vector2{static_cast<float>(width), static_cast<float>(height)};
//}
//
//int GlfwWindow::GetPixelWidth() const
//{
//	int width;
//	glfwGetFramebufferSize(m_glfwWindow, &width, nullptr);
//	return width;
//}
//
//void GlfwWindow::AddListener(IWindowListener* listener)
//{
//	m_listeners.insert(listener);
//}
//
//void GlfwWindow::RemoveListener(IWindowListener* listener)
//{
//	m_listeners.erase(listener);
//}
//
//void GlfwWindow::OnFramebufferSizeCallback(GLFWwindow *window, int width, int height)
//{
//    float scale = m_instance->GetLayoutScale();
//    
//	for (auto listener : m_instance->m_listeners)
//		listener->OnFrameBufferChanged((float)width / scale, (float)height / scale);
//}
//
//void GlfwWindow::OnSizeCallback(GLFWwindow *window, int width, int height)
//{
//	for (auto listener : m_instance->m_listeners)
//		listener->OnSizeChanged(width, height);
//}
//
//void GlfwWindow::OnCloseCallback(GLFWwindow *window)
//{
//	for (auto listener : m_instance->m_listeners)
//		listener->OnClosed();
//}
//
//int GlfwWindow::GetPixelHeight() const
//{
//	int height;
//	glfwGetFramebufferSize(m_glfwWindow, nullptr, &height);
//	return height;
//}
//
//float GlfwWindow::GetLayoutScale() const
//{
//    int framebuferWidth;
//    glfwGetFramebufferSize(m_glfwWindow, &framebuferWidth, nullptr);
//
//    int logicalWidth;
//    glfwGetWindowSize(m_glfwWindow, &logicalWidth, nullptr);
//
//    return logicalWidth > 0 ? (float)framebuferWidth / (float)logicalWidth : 1.f;
//}
//
//float GlfwWindow::GetAspectRatio() const
//{
//	return (float)GetPixelWidth() / (float)GetPixelHeight();
//}
//
//bool GlfwWindow::ShouldClose() const
//{
//	return glfwWindowShouldClose(m_glfwWindow);
//}
//
//void GlfwWindow::SwapBuffers()
//{
//	glfwSwapBuffers(m_glfwWindow);
//}
//
//void GlfwWindow::PollEvents()
//{
//	glfwPollEvents();
//}
