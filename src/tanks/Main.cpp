#include "app/tzod.h"
#include "app/View.h"
#include "as/AppConstants.h"
#include "filesystem/FileSystem.h"
#include "platglfw/GlfwAppWindow.h"
#include "platglfw/Timer.h"
#include "ui/ConsoleBuffer.h"

#include <exception>

namespace
{
	class ConsoleLog final
		: public UI::IConsoleLog
	{
		FILE *_file;
		ConsoleLog(const ConsoleLog&) = delete;
		ConsoleLog& operator= (const ConsoleLog&) = delete;
	public:
		explicit ConsoleLog(const char *filename)
			: _file(fopen(filename, "w"))
		{
		}
		virtual ~ConsoleLog()
		{
			if( _file )
				fclose(_file);
		}

		// IConsoleLog
		void WriteLine(int severity, const std::string &str) override
		{
			if( _file )
			{
				fputs(str.c_str(), _file);
				fputs("\n", _file);
				fflush(_file);
			}
			puts(str.c_str());
		}
		void Release() override
		{
			delete this;
		}
	};
}


static void print_what(UI::ConsoleBuffer &logger, const std::exception &e, std::string prefix = std::string());

static UI::ConsoleBuffer s_logger(100, 500);

//static long xxx = _CrtSetBreakAlloc(12649);

#include "audio/SoundEngine.hpp"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective

glm::mat4 camera(float Translate, const glm::vec2& Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windef.h>
#endif
int main(int, const char**)
try
{
    std::unique_ptr<SoundEngine> sound(new SoundEngine);
    sound->PlayOnce("file_example.wav");
    
    
	srand((unsigned int) time(nullptr));

#if defined(_DEBUG) && defined(_WIN32) // memory leaks detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	UI::ConsoleBuffer &logger = s_logger;

	logger.SetLog(new ConsoleLog("log.txt"));
	logger.Printf(0, "%s", TXT_VERSION);

	logger.Printf(0, "Mount file system");
	std::shared_ptr<FileSystem::IFileSystem> fs = FileSystem::CreateOSFileSystem("data");

	TzodApp app(*fs, logger);

	logger.Printf(0, "Create GL context");
	GlfwAppWindow appWindow(
		TXT_VERSION,
		false, // conf.r_fullscreen.Get(),
		1024, // conf.r_width.GetInt(),
		768 // conf.r_height.GetInt()
	);

    TzodView view(*fs, logger, app, appWindow);

	Timer timer;
	timer.SetMaxDt(0.05f);
	timer.Start();
	for (GlfwAppWindow::PollEvents(); !appWindow.ShouldClose(); GlfwAppWindow::PollEvents())
	{
		float dt = timer.GetDt();

		// controller pass
		view.Step(dt); // this also sends user controller state to WorldController
		app.Step(dt);

		// view pass
		view.Render(appWindow.GetPixelWidth(), appWindow.GetPixelHeight(), appWindow.GetLayoutScale());
		appWindow.Present();
	}

	app.Exit();

	logger.Printf(0, "Normal exit.");

	return 0;
}
catch (const std::exception &e)
{
	print_what(s_logger, e);
#ifdef _WIN32
	MessageBoxA(nullptr, e.what(), TXT_VERSION, MB_ICONERROR);
#endif
	return 1;
}

// recursively print exception whats:
static void print_what(UI::ConsoleBuffer &logger, const std::exception &e, std::string prefix)
{
#ifdef _WIN32
	OutputDebugStringA((prefix + e.what() + "\n").c_str());
#endif
	logger.Format(1) << prefix << e.what();
	try {
		std::rethrow_if_nested(e);
	}
	catch (const std::exception &nested) {
		print_what(logger, nested, prefix + "> ");
	}
}


