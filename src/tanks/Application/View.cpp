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

struct TzodViewImpl
{
	TextureManager textureManager;
	UI::LayoutManager gui;

	TzodViewImpl(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, Application &app, AppWindow &appWindow)
		: textureManager(InitTextureManager(fs, logger, appWindow.GetRender()))
		, gui(appWindow.GetInput(),
			appWindow.GetClipboard(),
			textureManager,
			DesktopFactory(//app.GetAppState(),
				//app.GetAppController(),
				fs,
				//app.GetConf(),
				//app.GetLang(),
				logger))
	{}
};

View::View(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, Application &app, AppWindow &appWindow)
	: _appWindow(appWindow)
	, _impl(new TzodViewImpl(fs, logger, app, appWindow))
{
	int width = appWindow.GetPixelWidth();
	int height = appWindow.GetPixelHeight();
	_impl->gui.GetDesktop()->Resize((float)width, (float)height);


	_appWindow.SetUserPointer(&_impl->gui);
}

View::~View()
{
	_appWindow.SetUserPointer(nullptr);
}

void View::Step(float dt)
{
	_impl->gui.TimeStep(dt); // this also sends user controller state to WorldController
}

void View::Render(AppWindow &appWindow)
{
	unsigned int width = appWindow.GetPixelWidth();
	unsigned int height = appWindow.GetPixelHeight();

	DrawingContext dc(_impl->textureManager, width, height);
	appWindow.GetRender().Begin();
	_impl->gui.Render(dc);
	appWindow.GetRender().End();
}

UI::LayoutManager& View::GetGui()
{
	return _impl->gui;
}

