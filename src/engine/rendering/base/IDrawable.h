#pragma once

class DrawingContext;

struct IDrawable
{
	virtual ~IDrawable() = default;

	virtual int GetOrder() const = 0;
	virtual void Draw(DrawingContext& dc, float interpolation) const = 0;
};
