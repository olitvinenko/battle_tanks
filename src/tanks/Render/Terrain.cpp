#include "Terrain.h"
#include "rendering/base/IRender.h"
#include "rendering/TextureManager.h"
#include "rendering/DrawingContext.h"

Terrain::Terrain(TextureManager &tm)
	: _texBack(tm.FindSprite("background"))
	, _texGrid(tm.FindSprite("grid"))
{
}

void Terrain::Draw(DrawingContext &dc, const RectFloat &bounds, bool drawGrid) const
{
	dc.DrawBackground(_texBack, bounds);
	if( drawGrid )
		dc.DrawBackground(_texGrid, bounds);
}
