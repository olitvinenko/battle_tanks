#pragma once
#include "ui/StackLayout.h"
#include <functional>

namespace FS
{
	class FileSystem;
}

namespace UI
{
	class ConsoleBuffer;
}

class LangCache;

struct MainMenuCommands
{
	std::function<void()> newCampaign;
	std::function<void()> newDM;
	std::function<void()> newMap;
	std::function<void()> openMap;
	std::function<void()> exportMap;
	std::function<void()> mapSettings;
	std::function<void()> gameSettings;
	std::function<void()> close;
};

class MainMenuDlg : public UI::StackLayout
{
public:
	MainMenuDlg(UI::LayoutManager &manager,
	            TextureManager &texman,
	            LangCache &lang,
	            MainMenuCommands commands);

private:
	LangCache &_lang;
	MainMenuCommands _commands;
};
