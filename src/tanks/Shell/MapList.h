#pragma once

#include "ui/List.h"

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ConsoleBuffer;
}

class ListDataSourceMaps : public UI::ListDataSourceDefault
{
public:
	ListDataSourceMaps(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger);
};

