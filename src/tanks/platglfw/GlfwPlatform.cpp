#include "GlfwPlatform.h"
#include "GlfwKeys.h"
#include "rendering/OpenGL.h"

Vector2 GetCursorPosInPixels(GLFWwindow *window, double dipX, double dipY)
{
	int pxWidth;
	int pxHeight;
	glfwGetFramebufferSize(window, &pxWidth, &pxHeight);

	int dipWidth;
	int dipHeight;
	glfwGetWindowSize(window, &dipWidth, &dipHeight);

	return{ float(dipX * pxWidth / dipWidth), float(dipY * pxHeight / dipHeight) };
}

Vector2 GetCursorPosInPixels(GLFWwindow *window)
{
    double dipX = 0;
    double dipY = 0;
    glfwGetCursorPos(window, &dipX, &dipY);
    return GetCursorPosInPixels(window, dipX, dipY);
}


GlfwInput::GlfwInput(GLFWwindow* window)
	: m_window(window)
{}

bool GlfwInput::IsKeyPressed(UI::Key key) const
{
	int platformKey = UnmapGlfwKeyCode(key);
	return GLFW_PRESS == glfwGetKey(m_window, platformKey);
}

bool GlfwInput::IsMousePressed(int button) const
{
	return GLFW_PRESS == glfwGetMouseButton(m_window, button-1);
}

Vector2 GlfwInput::GetMousePos() const
{
	return GetCursorPosInPixels(m_window);
}


GlfwClipboard::GlfwClipboard(GLFWwindow* window)
	: m_window(window)
{}

const char* GlfwClipboard::GetClipboardText() const
{
	return glfwGetClipboardString(m_window);
}

void GlfwClipboard::SetClipboardText(std::string text)
{
	glfwSetClipboardString(m_window, text.c_str());
}
