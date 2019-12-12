//#pragma once
//
//#include "Base/IInput.h"
//#include <memory>
//#include <set>
//
//class GlfwWindow;
//struct GLFWwindow;
//namespace UI {
//enum class Key;
//}
//enum class MouseButton;
//
//class GlfwInput final : public IInput
//{
//public:
//	explicit GlfwInput(std::shared_ptr<GlfwWindow> window);
//
//	void Read() override;
//
//	void AddListener(IInputListener *listener) override;
//
//	void RemoveListener(IInputListener *listener) override;
//
//	bool GetKey(UI::Key key) const override;
//	bool GetMouseButton(MouseButton button) const override;
//	const Vector2& GetMousePosition() const override;
//
//	~GlfwInput();
//
//	void Clear() override;
//
//private:
//	static void OnMouseButton(GLFWwindow *window, int button, int action, int mods);
//	static void OnKey(GLFWwindow *window, int platformKey, int scancode, int action, int mods);
//	static void OnScroll(GLFWwindow *window, double xOffset, double yOffset);
//	static void OnCursorPos(GLFWwindow *window, double xpos, double ypos);
//	static void OnChar(GLFWwindow *window, unsigned int codepoint);
//
//	std::shared_ptr<GlfwWindow> m_window;
//	std::set<IInputListener*> m_listeners;
//
//	static Vector2 m_mousePosition;
//	static bool m_mouseButtons[];
//	static bool m_keyboardButtons[];
//
//	static GlfwInput* m_instance;
//};
