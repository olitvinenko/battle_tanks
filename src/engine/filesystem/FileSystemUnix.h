#pragma once

#include "FileSystem.h"

namespace FileSystem
{
	class FileSystemUnix : public IFileSystem
	{
	public:
		explicit FileSystemUnix(const std::string& rootDirectory);

    protected:
		char Separator() const override;
		std::shared_ptr<IFileSystem> GetFileSystem(const std::string &path, bool create = false, bool nothrow = false) override;
		std::vector<std::string> EnumAllFiles(const std::string &mask) override;
	};
}
