#include <cassert>
#include <fstream>

#include "FileSystem.h"
#include "FileSystemWin32.h"

namespace FileSystem
{
	int toCppMode(FileOpenMode mode)
	{
		int result = 0;

		if ((mode & FileOpenMode::Read) == FileOpenMode::Read)
			result |= std::fstream::in;

		if ((mode & FileOpenMode::Write) == FileOpenMode::Write)
			result |= std::fstream::out;

		if ((mode & FileOpenMode::Append) == FileOpenMode::Append)
			result |= std::fstream::app;

		if ((mode & FileOpenMode::Binary) == FileOpenMode::Binary)
			result |= std::fstream::binary;

		if ((mode & FileOpenMode::AtEnd) == FileOpenMode::AtEnd)
			result |= std::fstream::ate;

		if ((mode & FileOpenMode::Truncate) == FileOpenMode::Truncate)
			result |= std::fstream::trunc;

		return result;
	}

	FileOpenMode operator |(FileOpenMode lhs, FileOpenMode rhs)
	{
		return static_cast<FileOpenMode> (
			static_cast<std::underlying_type<FileOpenMode>::type>(lhs) |
			static_cast<std::underlying_type<FileOpenMode>::type>(rhs)
			);
	}

	FileOpenMode operator &(FileOpenMode lhs, FileOpenMode rhs)
	{
		return static_cast<FileOpenMode> (
			static_cast<std::underlying_type<FileOpenMode>::type>(lhs) &
			static_cast<std::underlying_type<FileOpenMode>::type>(rhs)
			);
	}

	void IFileSystem::Mount(const std::string &nodeName, std::shared_ptr<IFileSystem> fs)
	{
		assert(!nodeName.empty() && std::string::npos == nodeName.find('/'));
		_children[nodeName] = fs;
	}

	std::shared_ptr<IFile> FileSystem::IFileSystem::Open(const std::string &fileName, FileOpenMode mode)
	{
		std::string::size_type pd = fileName.rfind('/');
		if (pd && std::string::npos != pd) // was a path delimiter found?
		{
			return GetFileSystem(fileName.substr(0, pd))->RawOpen(fileName.substr(pd + 1), mode);
		}
		return RawOpen(fileName, mode);
	}

	std::shared_ptr<IFileSystem> IFileSystem::GetFileSystem(const std::string &path, bool create, bool nothrow)
	{
		assert(!path.empty());

		// skip delimiters at the beginning
		std::string::size_type offset = path.find_first_not_of('/');
		assert(std::string::npos != offset);

		std::string::size_type p = path.find('/', offset);
		std::string dirName = path.substr(offset, std::string::npos != p ? p - offset : p);

		auto it = _children.find(dirName);
		if (_children.end() == it)
		{
			if (nothrow)
				return nullptr;
			else
				throw std::runtime_error("node not found in base file system");
		}

		if (std::string::npos != p)
			return it->second->GetFileSystem(path.substr(p), create, nothrow);

		return it->second;
	}
}