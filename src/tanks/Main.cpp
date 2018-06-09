#include "Config.h"
#include "FileSystem.h"
#include "GlfwAppWindow.h"
#include "ConsoleBuffer.h"

#include <exception>
#include <iostream>
#include "GameLoop.h"

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

class LoopTester
	: public IUpdatable
	, public IRenderable
	, public IFixedUpdatable
{
public:
	void Update(float realDeltaTime) override
	{
		// ai update
		// physics update

		std::cout << "Update \t realDeltaTime = " << realDeltaTime << std::endl;
	}

	void FixedUpdate(float fixedDeltaTime) override
	{
		std::cout << "FixedUpdate \t fixedDeltaTime = " << fixedDeltaTime << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); //for work simulation
	}

	void Render(float interpolation) override
	{
		std::cout << "Render \t interpolation = " << interpolation << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(20)); //for work simulation
		//view_position = position + (speed * interpolation)
	}
};


int main(int, const char**)
{
	try
	{
		srand(static_cast<unsigned int>(time(nullptr)));

#if defined(_DEBUG) && defined(_WIN32) // memory leaks detection
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		GameLoop loop;
		LoopTester tester;		
		loop.Add<IUpdatable>(&tester);
		loop.Add<IRenderable>(&tester);
		loop.Add<IFixedUpdatable>(&tester);

		std::shared_ptr<FileSystem::IFileSystem> fs = FileSystem::CreateOSFileSystem("data");

		UI::ConsoleBuffer &logger = s_logger;

		logger.SetLog(new ConsoleLog());
		logger.Printf(0, "%s", TXT_VERSION);

		logger.Printf(0, "Mount file system");
		

		//Application app(*fs, logger);

		logger.Printf(0, "Create GL context");
		GlfwAppWindow appWindow(TXT_VERSION, false, 1024, 768);

	//	View view(*fs, logger, app, appWindow);

		loop.Start();
		while (!appWindow.ShouldClose())
		{
			loop.Tick();

			appWindow.PollEvents();

			//float dt = timer.GetDt();

			// controller pass
			//view.Step(dt); // this also sends user controller state to WorldController
			//app.Step(dt);

			// view pass
			//view.Render(appWindow);
			appWindow.Present();
		}

		//app.Exit();

		loop.Remove<IUpdatable>(&tester);
		loop.Remove<IRenderable>(&tester);
		loop.Remove<IFixedUpdatable>(&tester);

		logger.Printf(0, "Normal exit.");

		return 0;
	}
	catch (const std::exception &e)
	{
		print_what(s_logger, e);
		return 1;
	}
}
