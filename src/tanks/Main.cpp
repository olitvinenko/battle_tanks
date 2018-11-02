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

#include "GlfwWindow.h"
#include "GlfwClipboard.h"
#include "GlfwInput.h"
#include "LoadDataState.h"

#include "GameStatesController.h"
#include "Rendering/RenderOpenGL.h"

#include "GameController.h"
#include "LayoutManager.h"

#include "MainMenu.h"

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

		std::shared_ptr<GlfwWindow> window = std::make_shared<GlfwWindow>(TXT_VERSION, 1024, 768, false);

		std::shared_ptr<IClipboard> clipboard = std::make_shared<GlfwClipboard>(window);
		std::shared_ptr<IInput> input = std::make_shared<GlfwInput>(window);

		RenderOpenGL render;

		Engine tanksEngine(window, clipboard, input, fs, &render, (int)DrawingOrder::Last);

		GameController game(tanksEngine);
		game.Launch();

		//std::shared_ptr<MainMenuDlg> d = std::make_shared<MainMenuDlg>(nullptr, *fs, logger, MainMenuCommands());

		// engine loop
		tanksEngine.Launch();

		game.Shutdown();
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
