#include "View.h"
#include "AppWindow.h"
#include "BattleTanks.h"
#include "Config.h"
#include "FileSystem.h"
#include "ConsoleBuffer.h"
#include "UIWindow.h"
#include "TextureManager.h"
#include "DrawingContext.h"
#include "GuiManager.h"
#include "DesktopFactory.h"

static TextureManager InitTextureManager(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, IRender &render)
{
	TextureManager textureManager(render);

	if (textureManager.LoadPackage(FILE_TEXTURES, fs.Open(FILE_TEXTURES)->QueryMap(), fs) <= 0)
		logger.Printf(1, "WARNING: no textures loaded");
	if (textureManager.LoadDirectory(DIR_SKINS, "skin/", fs) <= 0)
		logger.Printf(1, "WARNING: no skins found");

	return textureManager;
}

View::View(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, Application &app, AppWindow &appWindow)
	: _appWindow(appWindow)
	, textureManager(InitTextureManager(fs, logger, appWindow.GetRender()))
	, gui(appWindow.GetInput(),
		appWindow.GetClipboard(),
		textureManager,
		DesktopFactory(fs, logger))
{
	int width = appWindow.GetPixelWidth();
	int height = appWindow.GetPixelHeight();
	gui.GetDesktop()->Resize((float)width, (float)height);

	_appWindow.SetUserPointer(&gui);
}

View::~View()
{
	_appWindow.SetUserPointer(nullptr);
}

void View::Step(float dt)
{
	gui.TimeStep(dt); // this also sends user controller state to WorldController
}

void View::Render(AppWindow &appWindow) const
{
	unsigned int width = appWindow.GetPixelWidth();
	unsigned int height = appWindow.GetPixelHeight();

	DrawingContext dc(textureManager, width, height);
	appWindow.GetRender().Begin();
	gui.Render(dc);
	appWindow.GetRender().End();
}

UI::LayoutManager& View::GetGui()
{
	return gui;
}

