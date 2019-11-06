#pragma once
#include "ObjectView.h"
#include <stddef.h>

class TextureManager;

class R_Booster : public ObjectRFunc
{
public:
	R_Booster(TextureManager &tm);
	void Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const override;

private:
	size_t _texId;
};

