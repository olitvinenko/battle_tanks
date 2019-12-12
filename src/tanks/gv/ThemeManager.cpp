#include "ThemeManager.h"
#include "as/AppConstants.h"
#include "as/AppState.h"
#include "ctx/GameContext.h"
#include "filesystem/FileSystem.h"
#include "gc/World.h"
#include "rendering/TextureManager.h"

ThemeManager::ThemeManager(AppState &appState, FileSystem::IFileSystem &fs, TextureManager &tm)
	: AppStateListener(appState)
	, _fs(fs)
	, _textureManager(tm)
{
	std::shared_ptr<FileSystem::IFileSystem> dir = _fs.GetFileSystem(DIR_THEMES);
	auto files = dir->EnumAllFiles("*.lua");
	for( auto it = files.begin(); it != files.end(); ++it )
	{
		ThemeDesc td;
		td.fileName = std::move(*it);
		td.file = dir->Open(td.fileName)->AsMemory();
		_themes.push_back(td);
	}
}

ThemeManager::~ThemeManager()
{
}

size_t ThemeManager::GetThemeCount() const
{
	return _themes.size() + 1;
}

std::string ThemeManager::GetThemeName(size_t index) const
{
	if( 0 == index )
		return "<standard>";
	return _themes[index-1].fileName.substr(0, _themes[index-1].fileName.size() - 4); // throw off the extension
}

void ThemeManager::OnGameContextChanging()
{
}

void ThemeManager::OnGameContextChanged()
{
	// load default theme
	_textureManager.LoadPackage(ParsePackage(FILE_TEXTURES, _fs.Open(FILE_TEXTURES)->AsMemory(), _fs));

	if (GameContextBase *gameContext = GetAppState().GetGameContext())
	{
		// start from 1 to skip default
		for (size_t i = 1; i < GetThemeCount(); ++i)
		{
			if (GetThemeName(i) == gameContext->GetWorld()._infoTheme)
			{
				// _themes there is no entry for default

				_textureManager.LoadPackage(ParsePackage(_themes[i - 1].fileName, _themes[i - 1].file, _fs));
			}
		}
	}
}

