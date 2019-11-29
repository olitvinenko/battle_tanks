#pragma once
#include "as/AppStateListener.h"
#include <memory>
#include <string>
#include <vector>

class TextureManager;
namespace FileSystem
{
	class IFileSystem;
	class Memory;
}

class ThemeManager : private AppStateListener
{
public:
	ThemeManager(AppState &appState, FileSystem::IFileSystem &fs, TextureManager &tm);
	~ThemeManager();

	size_t GetThemeCount() const;
	std::string GetThemeName(size_t index) const;

private:
	struct ThemeDesc
	{
		std::string fileName;
		std::shared_ptr<FileSystem::Memory> file;
	};

	std::vector<ThemeDesc> _themes;
	FileSystem::IFileSystem &_fs;
	TextureManager &_textureManager;

	// AppStateListener
	void OnGameContextChanging() override;
	void OnGameContextChanged() override;

	ThemeManager(const ThemeManager&) = delete;
};
