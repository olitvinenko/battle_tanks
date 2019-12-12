#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

#include "FileSystemWin32.h"
#include <utf8.h>

namespace FileSystem
{
	//----------------------------------------------------------------------------------------------

	static std::wstring s2w(const std::string s)
	{
		std::wstring w;
		utf8::utf8to16(s.begin(), s.end(), std::back_inserter(w));
		return w;
	}

	static std::string w2s(const std::wstring w)
	{
		std::string s;
		utf8::utf16to8(w.begin(), w.end(), std::back_inserter(s));
		return s;
	}

	static std::string StrFromErr(DWORD dwMessageId)
	{
		WCHAR msgBuf[1024];
		DWORD msgSize = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			dwMessageId,
			0,
			msgBuf,
			1024,
			nullptr);

		while (msgSize)
		{
			if (msgBuf[msgSize - 1] == L'\n' || msgBuf[msgSize - 1] == L'\r')
			{
				--msgSize;
			}
			else
			{
				break;
			}
		}

		if (msgSize)
		{
			std::string result;
			utf8::utf16to8(msgBuf, msgBuf + msgSize, std::back_inserter(result));
			return result;
		}
		else
		{
			std::ostringstream ss;
			ss << "Unknown error (" << dwMessageId << ")";
			return ss.str();
		}
	}

	//----------------------------------------------------------------------------------------------

	FileSystemWin32::FileSystemWin32(std::string &&rootDirectory)
		: IFileSystem(std::move(rootDirectory))
	{
	}

	std::vector<std::string> FileSystemWin32::EnumAllFiles(const std::string &mask)
	{
		std::wstring query = s2w(_rootDirectory + Separator());
		utf8::utf8to16(mask.begin(), mask.end(), std::back_inserter(query));

		WIN32_FIND_DATAW fd;
		HANDLE hSearch = FindFirstFileExW(
			query.c_str(),
			FindExInfoStandard,
			&fd,
			FindExSearchNameMatch,
			nullptr,
			0);
		if (INVALID_HANDLE_VALUE == hSearch)
		{
			if (ERROR_FILE_NOT_FOUND == GetLastError())
			{
				return std::vector<std::string>(); // nothing matches
			}
			throw std::runtime_error(StrFromErr(GetLastError()));
		}

		std::vector<std::string> files;
		do
		{
			if (0 == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				files.push_back(w2s(fd.cFileName));
			}
		} while (FindNextFileW(hSearch, &fd));

		FindClose(hSearch);
		return files;
	}

	char FileSystemWin32::Separator() const
	{
		return '\\';
	}

	std::shared_ptr<IFileSystem> FileSystemWin32::GetFileSystem(const std::string &path, bool create, bool nothrow)
	{
		try
		{
			if (std::shared_ptr<IFileSystem> tmp = IFileSystem::GetFileSystem(path, create, true))
			{
				return tmp;
			}

			assert(!path.empty());

			// skip delimiters at the beginning
			std::string::size_type offset = path.find_first_not_of('/');
			assert(std::string::npos != offset);

			std::string::size_type p = path.find('/', offset);
			std::string dirName = path.substr(offset, std::string::npos != p ? p - offset : p);

			std::wstring tmpDir = s2w(_rootDirectory + Separator());
			utf8::utf8to16(dirName.begin(), dirName.end(), std::back_inserter(tmpDir));

			// try to find directory
			WIN32_FIND_DATAW fd = { 0 };
			HANDLE search = FindFirstFileExW(
				tmpDir.c_str(),
				FindExInfoStandard,
				&fd,
				FindExSearchNameMatch,
				nullptr,
				0);

			if (INVALID_HANDLE_VALUE != search)
			{
				FindClose(search);
			}
			else
			{
				if (create)
				{
					if (!CreateDirectoryW(tmpDir.c_str(), nullptr))
					{
						// creation failed
						if (nothrow)
							return nullptr;
						else
							throw std::runtime_error(StrFromErr(GetLastError()));
					}
					else
					{
						// try searching again to get attributes
						HANDLE search2 = FindFirstFileExW(
							tmpDir.c_str(),
							FindExInfoStandard,
							&fd,
							FindExSearchNameMatch,
							nullptr,
							0);
						FindClose(search2);
						if (INVALID_HANDLE_VALUE == search2)
						{
							if (nothrow)
								return nullptr;
							else
								throw std::runtime_error(StrFromErr(GetLastError()));
						}
					}
				}
				else
				{
					// directory not found
					if (nothrow)
						return nullptr;
					else
						throw std::runtime_error(StrFromErr(GetLastError()));
				}
			}

			if (0 == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				throw std::runtime_error("object is not a directory");

			std::shared_ptr<IFileSystem> child = std::make_shared<FileSystemWin32>(_rootDirectory + Separator() + dirName);
			Mount(dirName, child);

			if (std::string::npos != p)
				return child->GetFileSystem(path.substr(p), create, nothrow); // process the rest of the path
			return child; // last path node was processed
		}
		catch (const std::exception&)
		{
			std::ostringstream ss;
			ss << "Failed to open directory '" << path << "'";
			std::throw_with_nested(std::runtime_error(ss.str()));
		}
	}

	std::shared_ptr<IFileSystem> CreateOSFileSystem(const std::string &rootDirectory)
	{
		// convert to absolute path
		std::wstring tmpRel = s2w(rootDirectory);
		if (DWORD len = GetFullPathNameW(tmpRel.c_str(), 0, nullptr, nullptr))
		{
			std::wstring tmpFull(len, L'\0');
			if (DWORD len2 = GetFullPathNameW(tmpRel.c_str(), len, &tmpFull[0], nullptr))
			{
				tmpFull.resize(len2); // truncate terminating \0
				return std::make_shared<FileSystemWin32>(std::move(w2s(tmpFull)));
			}
		}

		throw std::runtime_error(StrFromErr(GetLastError()));
	}
}
