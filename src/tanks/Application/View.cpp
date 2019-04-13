#include "View.h"
#include "AppWindowBase.h"
#include "AppController.h"
#include "Config.h"
#include "FileSystem.h"
#include "ConsoleBuffer.h"
#include "UIWindow.h"
#include "GuiManager.h"
#include "Rendering/DrawingContext.h"

static TextureManager InitTextureManager(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, IRender &render)
{
	TextureManager textureManager(&render);

	if (textureManager.LoadPackage(FILE_TEXTURES, fs.Open(FILE_TEXTURES)->AsMemory(), fs) <= 0)
		logger.Printf(1, "WARNING: no textures loaded");
	if (textureManager.LoadDirectory(DIR_SKINS, "skin/", fs) <= 0)
		logger.Printf(1, "WARNING: no skins found");

	return textureManager;
}

View::View(GameLoop &loop,  FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, AppController &controller, AppWindowBase &appWindow)
	: m_loop(loop)
	, m_appWindow(appWindow)
	, m_textureManager(InitTextureManager(fs, logger, appWindow.GetRender()))
	, m_gui(appWindow.GetInput(), appWindow.GetClipboard(),	m_textureManager, controller)
{
	m_loop.Add<IFixedUpdatable>(this);
	m_loop.Add<IRenderable>(this);

	int width = appWindow.GetPixelWidth();
	int height = appWindow.GetPixelHeight();
	m_gui.GetDesktop()->Resize(static_cast<float>(width), static_cast<float>(height));

	m_appWindow.SetUserPointer(&m_gui);
}

View::~View()
{
	m_loop.Remove<IRenderable>(this);
	m_loop.Remove<IFixedUpdatable>(this);

	m_appWindow.SetUserPointer(nullptr);
}

void View::FixedUpdate(float fixedDeltaTime)
{
	m_gui.Update(fixedDeltaTime); // this also sends user controller state to WorldController
}

void View::Render(float interpolation) const
{
	unsigned int width = m_appWindow.GetPixelWidth();
	unsigned int height = m_appWindow.GetPixelHeight();

	IRender& r = m_appWindow.GetRender();

	DrawingContext dc(m_textureManager, &r, width, height);
	m_appWindow.GetRender().Begin();
	m_gui.Draw(dc, interpolation);
	m_appWindow.GetRender().End();
}

UI::LayoutManager& View::GetGui()
{
	return m_gui;
}

