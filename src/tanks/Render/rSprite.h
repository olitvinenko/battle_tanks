#pragma once
#include "ObjectView.h"
#include <stddef.h>

class TextureManager;

class R_Sprite : public ObjectRFunc
{
public:
	R_Sprite(TextureManager &tm, const char *tex);
	void Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const override;

private:
	size_t _texId;
};

class Z_Const : public ObjectZFunc
{
public:
	Z_Const(enumZOrder z) : _z(z) {}
	enumZOrder GetZ(const World &world, const GC_Actor &actor) const override { return _z; }

private:
	enumZOrder _z;
};
