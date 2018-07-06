#include "Config.h"
#include "FileSystem.h"
#include "GlfwAppWindow.h"
#include "ConsoleBuffer.h"
#include "GameLoop.h"
#include "AppController.h"
#include "View.h"

#include <exception>
#include <iostream>

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
