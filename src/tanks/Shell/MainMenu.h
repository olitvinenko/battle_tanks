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

class MainMenuDlg : public UI::StackLayout
{
public:
    struct Commands
    {
        std::function<void()> newDM;
        std::function<void()> editor;
        std::function<void()> gameSettings;
        
        std::function<void()> newCampaign;
        std::function<void()> mapSettings;
    };
    
	MainMenuDlg(UI::LayoutManager &manager,
	            TextureManager &texman,
	            LangCache &lang,
	            Commands&& commands);

private:
	LangCache &_lang;
	Commands _commands;
};
