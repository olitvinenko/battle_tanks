#include "BotView.h"
#include "Configuration.h"
#include "ui/LayoutContext.h"
#include "video/TextureManager.h"
#include "video/DrawingContext.h"

BotView::BotView(UI::LayoutManager &manager, TextureManager &texman)
	: UI::Window(manager)
	, _texRank(texman.FindSprite("rank_mark"))
	, _font(texman.FindSprite("font_small"))
{
}

void BotView::SetBotConfig(VariableTable &botConf, TextureManager &texman)
{
	_botConfCache.reset(new ConfPlayerAI(&botConf));
	_texSkin = texman.FindSprite(std::string("skin/") + _botConfCache->skin.Get());
}

void BotView::Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const
{
	if (_botConfCache)
	{
		float pxTextHeight = ToPx(texman.GetSpriteInfo(_font).pxFrameHeight, lc);
		vec2d pxSize = UI::ToPx(vec2d{ 64, 64 }, lc);
		dc.DrawSprite(MakeRectWH(vec2d{0, -pxTextHeight}, pxSize), _texSkin, 0xffffffff, 0);
		dc.DrawBitmapText(vec2d{ std::floor(pxSize.x / 2), pxSize.y - pxTextHeight }, lc.GetScale(), _font, 0x7f7f7f7f, _botConfCache->nick.Get(), alignTextCB);
	}
}

