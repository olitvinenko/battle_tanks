#include "BattleTanks.h"
#include "ConsoleBuffer.h"

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

Application::TzodAppImpl::TzodAppImpl(FileSystem::IFileSystem& fs)
	//: m_appController(fs)
{	
}


Application::Application(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger)
	: m_logger(logger)
	, m_impl(new TzodAppImpl(fs))
{
	//LoadConfigNoThrow(m_impl->m_conf, logger, FILE_CONFIG),
	//LoadConfigNoThrow(m_impl->m_lang, logger, FILE_LANGUAGE),
	//setlocale(LC_CTYPE, m_impl->m_lang.c_locale.Get().c_str());
}

Application::~Application()
{
}

//AppState& Application::GetAppState() const
//{
//	return m_impl->m_appState;
//}
//
//AppController& Application::GetAppController() const
//{
//	return m_impl->m_appController;
//}
//
//ConfCache& Application::GetConf() const
//{
//	return m_impl->m_conf;
//}
//
//LangCache& Application::GetLang() const
//{
//	return m_impl->m_lang;
//}

void Application::Step(float dt) const
{
	//if (GameContextBase *gc = m_impl->m_appState.GetGameContext())
	//{
	//	gc->Step(dt);
	//}
}

void Application::Exit() const
{
	m_logger.Printf(0, "Saving config to '" FILE_CONFIG "'");
	//if (!m_impl->m_conf->Save(FILE_CONFIG))
	//{
	//	m_logger.Printf(1, "Failed to save config file");
	//}
}
