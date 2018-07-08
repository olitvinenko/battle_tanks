#pragma once

#include "FileSystem.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace FileSystem
{
	class FileSystemWin32 : public IFileSystem
	{
	public:
		explicit FileSystemWin32(std::string &&rootDirectory);

		char Separator() const override;
		std::shared_ptr<IFileSystem> GetFileSystem(const std::string &path, bool create = false, bool nothrow = false) override;
		std::vector<std::string> EnumAllFiles(const std::string &mask) override;
	};
}
