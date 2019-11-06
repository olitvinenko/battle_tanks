#include "GameClassVis.h"
#include <TypeSystem.h>
#include <WorldView.h>
#include <DataSource.h>
#include <LayoutContext.h>
#include <StateContext.h>
#include <DrawingContext.h>
#include <TextureManager.h>

GameClassVis::GameClassVis(UI::LayoutManager &manager, TextureManager &texman, WorldView &worldView)
	: UI::Window(manager)
	, _worldView(worldView)
	, _world(RectRB{-2, -2, 2, 2})
	, _texSelection(texman.FindSprite("ui/selection"))
{
}

void GameClassVis::SetGameClass(std::shared_ptr<UI::DataSource<const std::string&>> className)
{
	_className = std::move(className);
}

void GameClassVis::Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const
{
	if (!_className)
		return;

	_world.Clear();
	RTTypes::Inst().CreateActor(_world, RTTypes::Inst().GetTypeByName(_className->GetValue(sc)), vec2d{});

	RectRB viewport = { 0, 0, (int) lc.GetPixelSize().x, (int) lc.GetPixelSize().y };
	vec2d eye{ 0, 0 };
	float zoom = lc.GetScale();
	bool editorMode = true;
	bool drawGrid = false;
	bool nightMode = false;
	_worldView.Render(dc, _world, viewport, eye, zoom, editorMode, drawGrid, nightMode);

	FRECT sel = MakeRectRB(vec2d{}, lc.GetPixelSize());
	if (sc.GetState() == "Focused")
	{
		dc.DrawSprite(sel, _texSelection, 0xffffffff, 0);
		dc.DrawBorder(sel, _texSelection, 0xffffffff, 0);
	}
	else if (sc.GetState() == "Unfocused")
	{
		dc.DrawBorder(sel, _texSelection, 0xffffffff, 0);
	}
	else if (sc.GetState() == "Hover")
	{
		dc.DrawSprite(sel, _texSelection, 0xffffffff, 0);
	}
}
