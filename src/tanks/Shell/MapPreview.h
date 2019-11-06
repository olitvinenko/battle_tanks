#pragma once
#include <Window.h>
#include <memory>
#include <string>

class WorldView;
class MapCache;

namespace FileSystem
{
	class IFileSystem;
}
namespace UI
{
	class Rating;
	template<class T> struct DataSource;
}

class MapPreview: public UI::Window
{
public:
	MapPreview(UI::LayoutManager &manager, TextureManager &texman, FileSystem::IFileSystem &fs, WorldView &worldView, MapCache &mapCache);

	void SetMapName(std::shared_ptr<UI::DataSource<const std::string&>> mapName);
	void SetRating(std::shared_ptr<UI::DataSource<unsigned int>> rating);

	void SetPadding(float padding) { _padding = padding; }

	// UI::Window
	void Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const override;
	FRECT GetChildRect(TextureManager &texman, const UI::LayoutContext &lc, const UI::StateContext &sc, const UI::Window &child) const override;

private:
	FileSystem::IFileSystem &_fs;
	WorldView &_worldView;
	MapCache &_mapCache;
	size_t _font;
	size_t _texSelection;
	std::shared_ptr<UI::Rating> _rating;
	std::shared_ptr<UI::DataSource<const std::string&>> _mapName;
	float _padding = 0;
};
