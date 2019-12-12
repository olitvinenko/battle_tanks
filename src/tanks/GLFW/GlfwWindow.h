//#pragma once
//
//#include "Base/IWindow.h"
//#include <set>
//
//struct GLFWwindow;
//
//class GlfwWindow final : public IWindow
//{
//public:
//	GlfwWindow(const char* title, int width, int height, bool fullscreen);
//
//	~GlfwWindow() override;
//
//	int GetPixelWidth() const override;
//
//	void AddListener(IWindowListener* listener) override;
//
//	void RemoveListener(IWindowListener* listener) override;
//
//	int GetPixelHeight() const override;
//
//	float GetAspectRatio() const override;
//
//    float GetLayoutScale() const override;
//
//	bool ShouldClose() const override;
//
//	void SwapBuffers() override;
//
//	void PollEvents() override;
//
//	const std::string& GetName() const override
//	{
//		return m_name;
//	}
//
//    Vector2 GetCursorPosInPixels() const override;
//    Vector2 GetCursorPosInPixels(double xoffset, double yoffset) const override;
//    Vector2 GetPixelSize() const override;
//
//private:
//
//	static void OnFramebufferSizeCallback(GLFWwindow *window, int width, int height);
//	static void OnSizeCallback(GLFWwindow *window, int width, int height);
//	static void OnCloseCallback(GLFWwindow *window);
//
//	GLFWwindow* m_glfwWindow;
//	std::string m_name;
//	std::set<IWindowListener*> m_listeners;
//
//	friend class GlfwClipboard;
//	friend class GlfwInput;
//
//	static GlfwWindow* m_instance;
//};
