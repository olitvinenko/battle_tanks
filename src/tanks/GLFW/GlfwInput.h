#pragma once

#include "Base/IInput.h"

class GlfwWindow;
struct GLFWwindow;
enum class Key;
enum class MouseButton;

class GlfwInput final : public IInput
{
public:
	explicit GlfwInput(GlfwWindow& window);

	void Read() override;

	void Clear() override;

	bool GetKeyDown(Key key) const override;

	bool GetKey(Key key) const override;

	bool GetKeyUp(Key key) const override;

	bool GetMouseButton(MouseButton button) const override;

	bool GetMouseButtonUp(MouseButton button) const override;

	bool GetMouseButtonDown(MouseButton button) const override;

	const Vector2& GetMouseScrollOffset() const override;

	const Vector2& GetMousePosition() const override;

	const std::list<char>& GetCharacters() const override;

private:
	static void OnMouseButton(GLFWwindow *window, int button, int action, int mods);
	static void OnKey(GLFWwindow *window, int platformKey, int scancode, int action, int mods);
	static void OnScroll(GLFWwindow *window, double xOffset, double yOffset);
	static void OnCursorPos(GLFWwindow *window, double xpos, double ypos);
	static void OnChar(GLFWwindow *window, unsigned int codepoint);

	GlfwWindow& m_window;

	static Vector2 m_scrollOffset;
	static Vector2 m_mousePosition;
	static std::list<char> m_characters;

	static bool m_mouseButtons[];
	static bool m_mouseButtonsUp[];
	static bool m_mouseButtonsDown[];

	static bool m_keyboardButtons[];
	static bool m_keyboardButtonsUp[];
	static bool m_keyboardButtonsDown[];
};
