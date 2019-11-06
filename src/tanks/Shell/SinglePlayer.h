#pragma once
#include "MapCache.h"
#include <Dialog.h>
#include <ListBase.h>
#include <array>
#include <memory>
#include <vector>

class AppConfig;
class ShellConfig;
class LangCache;
class DMCampaign;
class MapPreview;
class World;
class WorldView;
namespace FileSystem
{
	class IFileSystem;
}
namespace UI
{
	class List;
	class StackLayout;
	class Text;
}

class SinglePlayer : public UI::Dialog
{
public:
	SinglePlayer(UI::LayoutManager &manager, TextureManager &texman, WorldView &worldView, FileSystem::IFileSystem &fs, AppConfig &appConfig, ShellConfig &conf, LangCache &lang, DMCampaign &dmCampaign);

	// UI::Window
	FRECT GetChildRect(TextureManager &texman, const UI::LayoutContext &lc, const UI::StateContext &sc, const UI::Window &child) const override;
	vec2d GetContentSize(TextureManager &texman, const UI::StateContext &sc, float scale) const override;

private:
	void OnOK();
	void OnCancel();
	void OnSelectMap(UI::LayoutManager &manager, TextureManager &texman, int index);

	ShellConfig &_conf;
	LangCache &_lang;
	DMCampaign &_dmCampaign;
	MapCache _mapCache;
	UI::ListDataSourceDefault _tilesSource;

	std::shared_ptr<UI::StackLayout> _content;

	std::shared_ptr<UI::Text> _tierTitle;
	std::shared_ptr<UI::List> _tiles;
	std::shared_ptr<UI::StackLayout> _description;
	std::shared_ptr<UI::StackLayout> _enemies;
};
