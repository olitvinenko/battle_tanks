#include "DataSource.h"
#include "Rating.h"
#include "LayoutContext.h"
#include "rendering/TextureManager.h"
#include "rendering/DrawingContext.h"

using namespace UI;

Rating::Rating(LayoutManager &manager, TextureManager &texman)
	: Window(manager)
	, _texture(texman.FindSprite("ui/star"))
{
}

void Rating::SetRating(std::shared_ptr<DataSource<unsigned int>> rating)
{
	_rating = std::move(rating);
}

void Rating::Draw(const StateContext &sc, const LayoutContext &lc, const InputContext &ic, DrawingContext &dc, TextureManager &texman) const
{
	auto &spriteInfo = texman.GetSpriteInfo(_texture);
	Vector2 spriteSize = { spriteInfo.pxFrameWidth, spriteInfo.pxFrameHeight };

	Vector2 scale = lc.GetPixelSize() / Vector2{ spriteSize.x * _maxRating, spriteSize.y };
	float minScale = std::min(scale.x, scale.y);

	Vector2 pxItemSize = ToPx(spriteSize, minScale);

	unsigned int rating = _rating ? _rating->GetValue(sc) : 0;
	for (unsigned int i = 0; i < _maxRating; i++)
	{
		dc.DrawSprite(MakeRectWH(Vector2{ pxItemSize.x * (float)i, 0 }, pxItemSize), _texture, 0xffffffff, i >= rating);
	}
}

Vector2 Rating::GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const
{
	auto &spriteInfo = texman.GetSpriteInfo(_texture);
	Vector2 spriteSize = { spriteInfo.pxFrameWidth, spriteInfo.pxFrameHeight };
	return Vec2dMulX(ToPx(spriteSize, scale), (float) _maxRating);
}

