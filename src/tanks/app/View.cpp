#include "View.h"
#include "tzod.h"
#include <AppConstants.h>
#include <FileSystem.h>
#include <Desktop.h>
#include <AppWindow.h>
#include <ConsoleBuffer.h>
#include <GuiManager.h>
#include <InputContext.h>
#include <LayoutContext.h>
#include <StateContext.h>
#include <Window.h>
#include <DrawingContext.h>
#include <RenderOpenGL.h>
#include <TextureManager.h>


static TextureManager InitTextureManager(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, IRender &render)
{
	TextureManager textureManager(render);

	if (textureManager.LoadPackage(ParsePackage(FILE_TEXTURES, fs.Open(FILE_TEXTURES)->AsMemory(), fs)) <= 0)
		logger.Printf(1, "WARNING: no textures loaded");
	if (textureManager.LoadPackage(ParseDirectory(DIR_SKINS, "skin/", fs)) <= 0)
		logger.Printf(1, "WARNING: no skins found");

	return textureManager;
}

struct TzodViewImpl
{
	TextureManager textureManager;
	UI::InputContext inputContext;
	UI::LayoutManager gui;
	std::shared_ptr<UI::Window> desktop;
#ifndef NOSOUND
	//SoundView soundView;
#endif

	TzodViewImpl(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, TzodApp &app, AppWindow &appWindow)
		: textureManager(InitTextureManager(fs, logger, appWindow.GetRender()))
		, inputContext(appWindow.GetInput(), appWindow.GetClipboard())
		, gui(textureManager, inputContext)
		, desktop(std::make_shared<Desktop>(
			gui,
			textureManager,
			app.GetAppState(),
			app.GetAppConfig(),
			app.GetAppController(),
			fs,
			app.GetShellConfig(),
			app.GetLang(),
			app.GetDMCampaign(),
			logger))
#ifndef NOSOUND
		//, soundView(*fs.GetFileSystem(DIR_SOUND), logger, app.GetAppState())
#endif
	{
		gui.SetDesktop(desktop);
	}
};

static AppWindow& EnsureCurrent(AppWindow &appWindow)
{
	appWindow.MakeCurrent();
	return appWindow;
}

TzodView::TzodView(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, TzodApp &app, AppWindow &appWindow)
	: _appWindow(EnsureCurrent(appWindow))
	, _impl(new TzodViewImpl(fs, logger, app, appWindow))
{
	//	ThemeManager themeManager(appState, *fs, texman);

	_appWindow.SetInputSink(&_impl->gui);
}

TzodView::~TzodView()
{
	_appWindow.MakeCurrent();
	_appWindow.SetInputSink(nullptr);
}

void TzodView::Step(float dt)
{
	_appWindow.MakeCurrent();
	_impl->gui.TimeStep(dt); // this also sends user controller state to WorldController
}

void TzodView::Render(float pxWidth, float pxHeight, float scale)
{
	_appWindow.MakeCurrent();

#ifndef NOSOUND
//        vec2d pos(0, 0);
//        if (!_world.GetList(LIST_cameras).empty())
//        {
//            _world.GetList(LIST_cameras).for_each([&pos](ObjectList::id_type, GC_Object *o)
//            {
//                pos += static_cast<GC_Camera*>(o)->GetCameraPos();
//            });
//        }
//        soundView.SetListenerPos(pos);
	//_impl->soundView.Step();
#endif

	DrawingContext dc(_impl->textureManager, _appWindow.GetRender(), static_cast<unsigned int>(pxWidth), static_cast<unsigned int>(pxHeight));
	_appWindow.GetRender().Begin();

	UI::StateContext stateContext;
	UI::LayoutContext layoutContext(1.f, scale, vec2d{}, vec2d{ pxWidth, pxHeight }, _impl->gui.GetDesktop()->GetEnabled(stateContext));
	UI::RenderSettings rs{ _impl->gui.GetInputContext(), dc, _impl->textureManager };

	UI::RenderUIRoot(*_impl->gui.GetDesktop(), rs, layoutContext, stateContext);

#ifndef NDEBUG
	for (auto &id2pos : rs.ic.GetLastPointerLocation())
	{
		FRECT dst = { id2pos.second.x - 4, id2pos.second.y - 4, id2pos.second.x + 4, id2pos.second.y + 4 };
		rs.dc.DrawSprite(dst, 0U, 0xffffffff, 0U);
	}
#endif

	_appWindow.GetRender().End();
}

