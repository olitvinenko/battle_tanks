#pragma once
#include "ObjectView.h"

class TextureManager;

class R_Decoration : public ObjectRFunc
{
public:
	R_Decoration(TextureManager &tm);
	void Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const override;

private:
	TextureManager &_tm;
};

class Z_Decoration : public ObjectZFunc
{
public:
	enumZOrder GetZ(const World &world, const GC_Actor &actor) const override;
};
