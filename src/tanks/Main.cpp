#include "Config.h"
#include "FileSystem.h"
#include "GlfwAppWindow.h"
#include "ConsoleBuffer.h"
#include "GameLoop.h"
#include "AppController.h"
#include "View.h"

#include <exception>
#include "Engine.h"
#include "Keys.h"
#include "GlfwKeys.h"

#include "Base/MouseButton.h"
#include "Base/IInput.h"
#include "Base/IClipboard.h"
#include "Base/IWindow.h"
#include "GLFW/glfw3.h"

class GlfwWindow final : public IWindow
{
public:
	GlfwWindow(const char* title, int width, int height, bool fullscreen)
		: m_name(title)
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize OpenGL");
		}

		m_window = glfwCreateWindow(
			fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->width : width,
			fullscreen ? glfwGetVideoMode(glfwGetPrimaryMonitor())->height : height,
			title,
			fullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);

		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(1);
	}

	~GlfwWindow() override
	{
		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	int GetPixelWidth() const override
	{
		int width;
		glfwGetFramebufferSize(m_window, &width, nullptr);
		return width;
	}

	int GetPixelHeight() const override
	{
		int height;
		glfwGetFramebufferSize(m_window, nullptr, &height);
		return height;
	}

	float GetAspectRatio() const override
	{
		return (float)GetPixelWidth() / (float)GetPixelHeight();
	}

	bool ShouldClose() const override
	{
		return glfwWindowShouldClose(m_window);
	}

	void SwapBuffers() override
	{
		glfwSwapBuffers(m_window);
	}

	void PollEvents() override
	{
		glfwPollEvents();
	}

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

	friend class GlfwClipboard;
	friend class GlfwInput;

	std::string m_name;
};

class GlfwClipboard final : public IClipboard
{
public:
	explicit GlfwClipboard(GlfwWindow& window)
		: m_window(window)
	{ }

	const std::string& GetText() override
	{
		return glfwGetClipboardString(m_window.m_window);
	}

	void SetText(const std::string& text) override
	{
		glfwSetClipboardString(m_window.m_window, text.c_str());
	}

private:
	GlfwWindow& m_window;
};

class GlfwInput final : public IInput
{
public:
	explicit GlfwInput(GlfwWindow& window)
		: m_window(window)
	{
		GLFWwindow* glfwWindow = m_window.m_window;

		glfwSetMouseButtonCallback(glfwWindow, OnMouseButton);
		glfwSetCursorPosCallback(glfwWindow, OnCursorPos);
		glfwSetScrollCallback(glfwWindow, OnScroll);
		glfwSetKeyCallback(glfwWindow, OnKey);
		glfwSetCharCallback(glfwWindow, OnChar);
	}

	void Read() override
	{
		glfwPollEvents();
	}

	void Clear() override
	{
		memset(m_mouseButtonsUp, 0, static_cast<int>(MouseButton::Last) * sizeof(*m_mouseButtonsUp));
		memset(m_mouseButtonsDown, 0, static_cast<int>(MouseButton::Last) * sizeof(*m_mouseButtonsDown));
		
		memset(m_keyboardButtons, 0, static_cast<int>(Key::Last) * sizeof(*m_keyboardButtons));
		memset(m_keyboardButtonsUp, 0, static_cast<int>(Key::Last) * sizeof(*m_keyboardButtonsUp));
		memset(m_keyboardButtonsDown, 0, static_cast<int>(Key::Last) * sizeof(*m_keyboardButtonsDown));

		m_scrollOffset.set(.0f, .0f);
		m_characters.clear();
	}

	bool GetKeyDown(Key key) const override
	{
		return m_keyboardButtonsDown[static_cast<int>(key)];
	}

	bool GetKey(Key key) const override
	{
		return m_keyboardButtons[static_cast<int>(key)];
	}

	bool GetKeyUp(Key key) const override
	{
		return m_keyboardButtonsUp[static_cast<int>(key)];
	}

	bool GetMouseButton(MouseButton button) const override
	{
		return m_mouseButtons[static_cast<int>(button)];
	}

	bool GetMouseButtonUp(MouseButton button) const override
	{
		return m_mouseButtonsUp[static_cast<int>(button)];
	}

	bool GetMouseButtonDown(MouseButton button) const override
	{
		return m_mouseButtonsDown[static_cast<int>(button)];
	}

	const Vector2& GetMouseScrollOffset() const override
	{
		return m_scrollOffset;
	}

	const Vector2& GetMousePosition() const override
	{
		return m_mousePosition;
	}

	const std::list<char>& GetCharacters() const override
	{
		return m_characters;
	}

private:
	static void OnMouseButton(GLFWwindow *window, int button, int action, int mods)
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

	static void OnKey(GLFWwindow *window, int platformKey, int scancode, int action, int mods)
	{
		Key key = MapGlfwKeyCode(platformKey);
		const int index = static_cast<int>(key);

		m_keyboardButtons[index] = action == GLFW_REPEAT;
		m_keyboardButtonsUp[index] = action == GLFW_RELEASE;
		m_keyboardButtonsDown[index] = action == GLFW_PRESS;
	}

	static void OnScroll(GLFWwindow *window, double xOffset, double yOffset)
	{
		int pxWidth;
		int pxHeight;
		glfwGetFramebufferSize(window, &pxWidth, &pxHeight);

		int dipWidth;
		int dipHeight;
		glfwGetWindowSize(window, &dipWidth, &dipHeight);

		m_scrollOffset.set(float(xOffset * pxWidth / dipWidth), float(yOffset * pxHeight / dipHeight));
	}

	static void OnCursorPos(GLFWwindow *window, double xpos, double ypos)
	{
		int pxWidth;
		int pxHeight;
		glfwGetFramebufferSize(window, &pxWidth, &pxHeight);

		int dipWidth;
		int dipHeight;
		glfwGetWindowSize(window, &dipWidth, &dipHeight);

		m_mousePosition.set(float(xpos * pxWidth / dipWidth), float(ypos * pxHeight / dipHeight));
	}

	static void OnChar(GLFWwindow *window, unsigned int codepoint)
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

bool GlfwInput::m_mouseButtons[static_cast<int>(MouseButton::Last)] = { false };
bool GlfwInput::m_mouseButtonsUp[static_cast<int>(MouseButton::Last)] = { false };
bool GlfwInput::m_mouseButtonsDown[static_cast<int>(MouseButton::Last)] = { false };

bool GlfwInput::m_keyboardButtons[static_cast<int>(Key::Last)] = { false };
bool GlfwInput::m_keyboardButtonsUp[static_cast<int>(Key::Last)] = { false };
bool GlfwInput::m_keyboardButtonsDown[static_cast<int>(Key::Last)] = { false };

Vector2 GlfwInput::m_scrollOffset;
Vector2 GlfwInput::m_mousePosition;
std::list<char> GlfwInput::m_characters;

namespace
{
	class ConsoleLog final
		: public UI::IConsoleLog
	{
	public:
		ConsoleLog() { }

		void WriteLine(int severity, const std::string &str) override
		{
			std::cout << str << std::endl;
		}

		void Release() override
		{
		}

	private:
		
		ConsoleLog(const ConsoleLog&) = delete;
		ConsoleLog& operator= (const ConsoleLog&) = delete;
	};
}


static void print_what(UI::ConsoleBuffer &logger, const std::exception &e, std::string prefix = std::string())
{
	logger.Format(1) << prefix << e.what();

	try
	{
		rethrow_if_nested(e);
	}
	catch (const std::exception &nested)
	{
		print_what(logger, nested, prefix + "> ");
	}
}

static UI::ConsoleBuffer s_logger(100, 500);


int main(int, const char**)
{
	try
	{
		srand(static_cast<unsigned int>(time(nullptr)));

#if defined(_DEBUG) && defined(_WIN32) // memory leaks detection
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		UI::ConsoleBuffer &logger = s_logger;

		logger.SetLog(new ConsoleLog());
		logger.Printf(0, "%s", TXT_VERSION);

		logger.Printf(0, "Mount file system");
		std::shared_ptr<FileSystem::IFileSystem> fs = FileSystem::CreateOSFileSystem("data");

		//--------------------------------------------------------------------------------------------------
		GlfwWindow window(TXT_VERSION, 1024, 768, false);
		GlfwClipboard clipboard(window);
		GlfwInput input(window);

		Engine tanksEngine(&window, &clipboard, &input);
		tanksEngine.Launch();
		return 0;
		//--------------------------------------------------------------------------------------------------


		GameLoop loop;

		logger.Printf(0, "Create GL context");
		GlfwAppWindow appWindow(TXT_VERSION, false, 1024, 768);

		AppController controller(loop, *fs, logger);
		View view(loop, *fs, logger, controller, appWindow);

		loop.Start();
		while (!appWindow.ShouldClose())
		{
			appWindow.PollEvents();

			loop.Tick();

			appWindow.SwapBuffers();
		}

		controller.Exit();

		logger.Printf(0, "Normal exit.");

		return 0;
	}
	catch (const std::exception &e)
	{
		print_what(s_logger, e);
		return 1;
	}
}
