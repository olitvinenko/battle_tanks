#pragma once

#include "Base/IWindow.h"

struct GLFWwindow;

class GlfwWindow final : public IWindow
{
public:
	GlfwWindow(const char* title, int width, int height, bool fullscreen);

	~GlfwWindow() override;

	int GetPixelWidth() const override;

	int GetPixelHeight() const override;

	float GetAspectRatio() const override;

	bool ShouldClose() const override;

	void SwapBuffers() override;

	void PollEvents() override;

	const std::string& GetName() const override
	{
		return m_name;
	}

	GLFWwindow* GetWindow() const
	{
		return m_window;
	}

private:
	GLFWwindow* m_window;
	std::string m_name;

	friend class GlfwClipboard;
	friend class GlfwInput;
};