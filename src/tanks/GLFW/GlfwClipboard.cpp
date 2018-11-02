#include "GlfwClipboard.h"
#include "GlfwWindow.h"
#include "GLFW/glfw3.h"

GlfwClipboard::GlfwClipboard(std::shared_ptr<GlfwWindow> window)
	: m_window(window)
{ }

std::string GlfwClipboard::GetText()
{
	return glfwGetClipboardString(m_window->m_window);
}

void GlfwClipboard::SetText(const std::string& text)
{
	glfwSetClipboardString(m_window->m_window, text.c_str());
}