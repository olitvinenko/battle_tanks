#include "GlfwInput.h"
#include "GlfwWindow.h"

#include "Base/MouseButton.h"
#include "Keys.h"
#include "Vector2.h"
#include "GlfwKeys.h"

#include <GLFW/glfw3.h>

bool GlfwInput::m_mouseButtons[static_cast<int>(MouseButton::Last)] = { false };
bool GlfwInput::m_mouseButtonsUp[static_cast<int>(MouseButton::Last)] = { false };
bool GlfwInput::m_mouseButtonsDown[static_cast<int>(MouseButton::Last)] = { false };

bool GlfwInput::m_keyboardButtons[static_cast<int>(Key::Last)] = { false };
bool GlfwInput::m_keyboardButtonsUp[static_cast<int>(Key::Last)] = { false };
bool GlfwInput::m_keyboardButtonsDown[static_cast<int>(Key::Last)] = { false };

Vector2 GlfwInput::m_scrollOffset;
Vector2 GlfwInput::m_mousePosition;
std::list<char> GlfwInput::m_characters;

GlfwInput::GlfwInput(std::shared_ptr<GlfwWindow> window)
	: m_window(window)
{
	GLFWwindow* glfwWindow = m_window->m_window;

	glfwSetMouseButtonCallback(glfwWindow, OnMouseButton);
	glfwSetCursorPosCallback(glfwWindow, OnCursorPos);
	glfwSetScrollCallback(glfwWindow, OnScroll);
	glfwSetKeyCallback(glfwWindow, OnKey);
	glfwSetCharCallback(glfwWindow, OnChar);
}

void GlfwInput::Read()
{
	m_window->PollEvents();
}

void GlfwInput::Clear()
{
	memset(m_mouseButtonsUp, 0, static_cast<int>(MouseButton::Last) * sizeof(*m_mouseButtonsUp));
	memset(m_mouseButtonsDown, 0, static_cast<int>(MouseButton::Last) * sizeof(*m_mouseButtonsDown));

	memset(m_keyboardButtons, 0, static_cast<int>(Key::Last) * sizeof(*m_keyboardButtons));
	memset(m_keyboardButtonsUp, 0, static_cast<int>(Key::Last) * sizeof(*m_keyboardButtonsUp));
	memset(m_keyboardButtonsDown, 0, static_cast<int>(Key::Last) * sizeof(*m_keyboardButtonsDown));

	m_scrollOffset.set(.0f, .0f);
	m_characters.clear();
}

bool GlfwInput::GetKeyDown(Key key) const
{
	return m_keyboardButtonsDown[static_cast<int>(key)];
}

bool GlfwInput::GetKey(Key key) const
{
	return m_keyboardButtons[static_cast<int>(key)];
}

bool GlfwInput::GetKeyUp(Key key) const
{
	return m_keyboardButtonsUp[static_cast<int>(key)];
}

bool GlfwInput::GetMouseButton(MouseButton button) const
{
	return m_mouseButtons[static_cast<int>(button)];
}

bool GlfwInput::GetMouseButtonUp(MouseButton button) const
{
	return m_mouseButtonsUp[static_cast<int>(button)];
}

bool GlfwInput::GetMouseButtonDown(MouseButton button) const
{
	return m_mouseButtonsDown[static_cast<int>(button)];
}

const Vector2& GlfwInput::GetMouseScrollOffset() const
{
	return m_scrollOffset;
}

const Vector2& GlfwInput::GetMousePosition() const
{
	return m_mousePosition;
}

const std::list<char>& GlfwInput::GetCharacters() const
{
	return m_characters;
}

// static members

void GlfwInput::OnMouseButton(GLFWwindow *window, int button, int action, int mods)
{
	MouseButton unmapped;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		unmapped = MouseButton::Left;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		unmapped = MouseButton::Right;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		unmapped = MouseButton::Middle;
		break;
	default: return;
	}

	const int index = static_cast<int>(unmapped);

	m_mouseButtonsDown[index] = action == GLFW_PRESS;
	m_mouseButtonsUp[index] = action == GLFW_RELEASE;
	m_mouseButtons[index] = action == GLFW_PRESS;
}

void GlfwInput::OnKey(GLFWwindow *window, int platformKey, int scancode, int action, int mods)
{
	Key key = MapGlfwKeyCode(platformKey);
	const int index = static_cast<int>(key);

	m_keyboardButtons[index] = action == GLFW_REPEAT;
	m_keyboardButtonsUp[index] = action == GLFW_RELEASE;
	m_keyboardButtonsDown[index] = action == GLFW_PRESS;
}

void GlfwInput::OnScroll(GLFWwindow *window, double xOffset, double yOffset)
{
	int pxWidth;
	int pxHeight;
	glfwGetFramebufferSize(window, &pxWidth, &pxHeight);

	int dipWidth;
	int dipHeight;
	glfwGetWindowSize(window, &dipWidth, &dipHeight);

	m_scrollOffset.set(float(xOffset * pxWidth / dipWidth), float(yOffset * pxHeight / dipHeight));
}

void GlfwInput::OnCursorPos(GLFWwindow *window, double xpos, double ypos)
{
	int pxWidth;
	int pxHeight;
	glfwGetFramebufferSize(window, &pxWidth, &pxHeight);

	int dipWidth;
	int dipHeight;
	glfwGetWindowSize(window, &dipWidth, &dipHeight);

	m_mousePosition.set(float(xpos * pxWidth / dipWidth), float(ypos * pxHeight / dipHeight));
}

void GlfwInput::OnChar(GLFWwindow *window, unsigned int codepoint)
{
	const char character = static_cast<char>(codepoint);

	if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9'))
	{
		m_characters.push_back(character);
		return;
	}

	switch (character)
	{
	case ' ':
	case '_':
	case '-':
		m_characters.push_back(character);
		break;

	default: break;
	}
}