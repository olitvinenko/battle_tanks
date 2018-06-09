#pragma once
#include "UIInput.h"
#include "Clipboard.h"

struct GLFWwindow;

class GlfwInput : public UI::IInput
{
public:
	GlfwInput(GLFWwindow &window);

	// IInput
	bool IsKeyPressed(UI::Key key) const override;
	bool IsMousePressed(int button) const override;
	math::Vector2 GetMousePos() const override;

private:
	GLFWwindow &m_window;
};

class GlfwClipboard : public UI::IClipboard
{
public:
	GlfwClipboard(GLFWwindow &window);

	// IClipboard
	const char* GetClipboardText() const override;
	void SetClipboardText(std::string text) override;

private:
	GLFWwindow &m_window;
};

math::Vector2 GetCursorPosInPixels(GLFWwindow *window);
math::Vector2 GetCursorPosInPixels(GLFWwindow *window, double dipX, double dipY);
