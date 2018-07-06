#include "AppController.h"
#include "ConsoleBuffer.h"
#include "GameContextBase.h"

#define FILE_CONFIG      "config.cfg"
#define FILE_LANGUAGE    "data/lang.cfg"

template <class T>
static void LoadConfigNoThrow(T &confRoot, UI::ConsoleBuffer &logger, const char *filename)
{
	try
	{
		logger.Printf(0, "Loading config '%s'", filename);
		if (!confRoot->Load(filename))
		{
			logger.Format(1) << "Failed to load a config file; using defaults";
		}
	}
	catch (const std::exception &e)
	{
		logger.Printf(1, "Could not load config '%s': %s", filename, e.what());
	}
}

AppController::AppController(GameLoop &loop, FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger)
	: m_loop(loop)
	, m_fs(fs)
	, m_logger(logger)
{
	m_loop.Add<IFixedUpdatable>(this);

	//LoadConfigNoThrow(m_impl->m_conf, logger, FILE_CONFIG),
	//LoadConfigNoThrow(m_impl->m_lang, logger, FILE_LANGUAGE),
	//setlocale(LC_CTYPE, m_impl->m_lang.c_locale.Get().c_str());
}

AppController::~AppController()
{
	m_loop.Remove<IFixedUpdatable>(this);
}

AppState& AppController::GetAppState()
{
	return m_appState;
}

void AppController::FixedUpdate(float fixedDeltaTime)
{
	if (GameContextBase *gc = m_appState.GetGameContext())
	{
		gc->FixedUpdate(fixedDeltaTime);
	}
}

void AppController::Exit() const
{
	m_logger.Printf(0, "Saving config to '" FILE_CONFIG "'");
}
