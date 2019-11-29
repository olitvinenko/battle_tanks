#pragma once
#include "ui/Window.h"
#include <memory>

class VariableTable;
class ConfPlayerAI;

class BotView : public UI::Window
{
public:
	BotView(UI::LayoutManager &manager, TextureManager &texman);

	void SetBotConfig(VariableTable &botConf, TextureManager &texman);

	// UI::Window
	void Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const override;

private:
	size_t _texSkin = -1;
	size_t _texRank;
	size_t _font;

	std::unique_ptr<ConfPlayerAI> _botConfCache;
};
