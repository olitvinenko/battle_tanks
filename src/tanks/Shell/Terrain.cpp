#include "Terrain.h"
#include "RenderBase.h"
#include "TextureManager.h"
#include "DrawingContext.h"

Terrain::Terrain(TextureManager &tm)
	: _texBack(tm.FindSprite("background"))
	, _texGrid(tm.FindSprite("grid"))
{
}

void Terrain::Draw(DrawingContext &dc, float sizeX, float sizeY, bool drawGrid) const
{
	dc.DrawBackground(_texBack, sizeX, sizeY);
	if( drawGrid )
		dc.DrawBackground(_texGrid, sizeX, sizeY);
}
