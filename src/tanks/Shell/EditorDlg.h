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

class EditorDlg : public UI::StackLayout
{
public:
    struct Commands
    {
        std::function<void()> newMap;
        std::function<void()> openMap;
        std::function<void()> saveMap;
    };
    
    
	EditorDlg(UI::LayoutManager& manager,
	            TextureManager& texman,
	            LangCache& lang,
                Commands&& commands);
    
private:
	LangCache &_lang;
    Commands _commands;
};
