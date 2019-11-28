#pragma once
#include <Window.h>
#include <memory>

class VariableTable;
class ConfPlayerLocal;

class PlayerView : public UI::Window
{
public:
	PlayerView(UI::LayoutManager &manager, TextureManager &texman);

	void SetPlayerConfig(VariableTable &playerConf, TextureManager &texman);

	// UI::Window
	void Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const override;

private:
	size_t _texSkin = -1;

	std::unique_ptr<ConfPlayerLocal> _playerConfCache;
};
