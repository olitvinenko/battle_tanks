#pragma once

#include "ui/UIInput.h"
#include "ui/Clipboard.h"

struct GLFWwindow;

class GlfwInput : public UI::IInput
{
public:
	GlfwInput(GLFWwindow* window);

	// UI::IInput
	bool IsKeyPressed(UI::Key key) const override;
	bool IsMousePressed(int button) const override;
	Vector2 GetMousePos() const override;

private:
	GLFWwindow* m_window;
};

class GlfwClipboard : public UI::IClipboard
{
public:
	GlfwClipboard(GLFWwindow* window);

	// UI::IClipboard
	const char* GetClipboardText() const override;
	void SetClipboardText(std::string text) override;

private:
	GLFWwindow* m_window;
};

Vector2 GetCursorPosInPixels(GLFWwindow *window);
Vector2 GetCursorPosInPixels(GLFWwindow *window, double dipX, double dipY);
