#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
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

	FileSystemWin32::OSFile::OSFile(std::wstring &&fileName, FileOpenMode mode)
		: _mode(mode)
		, _mapped(false)
		, _streamed(false)
	{
		//assert(_mode);

		std::replace(fileName.begin(), fileName.end(), L'/', L'\\');

		DWORD dwDesiredAccess = 0;
		DWORD dwShareMode = FILE_SHARE_READ;
		DWORD dwCreationDisposition;

		if ((_mode & FileOpenMode::Write) == FileOpenMode::Write)
		{
			dwDesiredAccess |= FILE_WRITE_DATA;
			dwShareMode = 0;
			dwCreationDisposition = CREATE_ALWAYS;
		}

		if ((_mode & FileOpenMode::Read) == FileOpenMode::Read)
		{
			dwDesiredAccess |= FILE_READ_DATA;
			dwCreationDisposition = ((_mode & FileOpenMode::Write) == FileOpenMode::Write) ? OPEN_ALWAYS : OPEN_EXISTING;
		}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		_file.h = ::CreateFileW(fileName.c_str(),
			dwDesiredAccess,
			dwShareMode,
			nullptr,                        // lpSecurityAttributes
			dwCreationDisposition,
			FILE_FLAG_SEQUENTIAL_SCAN,      // dwFlagsAndAttributes
			nullptr);                       // hTemplateFile
#elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PC_APP)
		_file.h = ::CreateFile2(fileName.c_str(),
			dwDesiredAccess,
			dwShareMode,
			dwCreationDisposition,
			nullptr);
#endif

		if (_file.h == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}
	}

	FileSystemWin32::OSFile::~OSFile()
	{
	}

	std::shared_ptr<IMemoryMap> FileSystemWin32::OSFile::QueryMap()
	{
		assert(!_mapped && !_streamed);
		std::shared_ptr<IMemoryMap> result = std::make_shared<OSMemMap>(shared_from_this(), _file.h);
		_mapped = true;
		return result;
	}

	std::shared_ptr<IStream> FileSystemWin32::OSFile::QueryStream()
	{
		assert(!_mapped && !_streamed);
		_streamed = true;
		std::shared_ptr<IStream> result = std::make_shared<OSStream>(shared_from_this(), _file.h);
		return result;
	}

	void FileSystemWin32::OSFile::Unmap()
	{
		assert(_mapped && !_streamed);
		_mapped = false;
	}

	void FileSystemWin32::OSFile::Unstream()
	{
		assert(_streamed && !_mapped);
		_streamed = false;
	}

	//----------------------------------------------------------------------------------------------

	FileSystemWin32::OSFile::OSStream::OSStream(std::shared_ptr<OSFile> parent, HANDLE hFile)
		: _file(parent)
		, _hFile(hFile)
	{
		Seek(0, SeekMethod::Begin);
	}

	FileSystemWin32::OSFile::OSStream::~OSStream()
	{
		_file->Unstream();
	}

	size_t FileSystemWin32::OSFile::OSStream::Read(void *dst, size_t size, size_t count)
	{
		DWORD bytesRead;
		if (!ReadFile(_hFile, dst, size * count, &bytesRead, nullptr))
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}
		if (bytesRead % size)
		{
			throw std::runtime_error("unexpected end of file");
		}
		return bytesRead / size;
	}

	void FileSystemWin32::OSFile::OSStream::Write(const void *src, size_t size)
	{
		DWORD written;
		BOOL result = WriteFile(_hFile, src, size, &written, nullptr);
		if (!result || written != size)
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}
	}

	void FileSystemWin32::OSFile::OSStream::Seek(long long amount, SeekMethod method)
	{
		DWORD dwMoveMethod;
		switch (method)
		{
		case SeekMethod::Begin: dwMoveMethod = FILE_BEGIN; break;
		case SeekMethod::Current: dwMoveMethod = FILE_CURRENT; break;
		case SeekMethod::End: dwMoveMethod = FILE_END; break;
		default:
			assert(false);
		}
		LARGE_INTEGER result;
		LARGE_INTEGER liAmount;
		liAmount.QuadPart = amount;
		if (!SetFilePointerEx(_hFile, liAmount, &result, dwMoveMethod))
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}
	}

	long long FileSystemWin32::OSFile::OSStream::Tell() const
	{
		LARGE_INTEGER zero = { 0 };
		LARGE_INTEGER current = { 0 };
		SetFilePointerEx(_hFile, zero, &current, FILE_CURRENT);
		return current.QuadPart;
	}

	//----------------------------------------------------------------------------------------------

	FileSystemWin32::OSFile::OSMemMap::OSMemMap(std::shared_ptr<OSFile> parent, HANDLE hFile)
		: _file(parent)
		, _hFile(hFile)
		, _data(nullptr)
		, _size(0)
	{
		SetupMapping();
	}

	FileSystemWin32::OSFile::OSMemMap::~OSMemMap()
	{
		if (_data)
		{
			UnmapViewOfFile(_data);
		}
		_file->Unmap();
	}

	void FileSystemWin32::OSFile::OSMemMap::SetupMapping()
	{
		LARGE_INTEGER size;
		if (!GetFileSizeEx(_hFile, &size))
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}

		if (size.HighPart > 0)
		{
			throw std::runtime_error("File is too large");
		}

		_size = size.LowPart;

		_map.h = CreateFileMapping(_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (nullptr == _map.h)
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}

		_data = MapViewOfFile(_map.h, FILE_MAP_READ, 0, 0, 0);
		if (nullptr == _data)
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}
	}

	char* FileSystemWin32::OSFile::OSMemMap::GetData()
	{
		return (char *)_data;
	}

	unsigned long FileSystemWin32::OSFile::OSMemMap::GetSize() const
	{
		return _size;
	}

	void FileSystemWin32::OSFile::OSMemMap::SetSize(unsigned long size)
	{
		BOOL bUnmapped = UnmapViewOfFile(_data);
		_data = nullptr;
		_size = 0;
		if (!bUnmapped)
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}

		CloseHandle(_map.h);

		LARGE_INTEGER distance = { size };
		if (!SetFilePointerEx(_hFile, distance, nullptr, FILE_BEGIN))
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}
		if (!SetEndOfFile(_hFile))
		{
			throw std::runtime_error(StrFromErr(GetLastError()));
		}

		SetupMapping();
		assert(_size == size);
	}

	//----------------------------------------------------------------------------------------------

	FileSystemWin32::FileSystemWin32(std::wstring &&rootDirectory)
		: _rootDirectory(std::move(rootDirectory))
	{
	}

	std::vector<std::string> FileSystemWin32::EnumAllFiles(const std::string &mask)
	{
		// query = _rootDirectory + '\\' + mask

		std::wstring query = _rootDirectory + L'\\';
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

	// open a file that strictly belongs to this file system
	std::shared_ptr<IFile> FileSystemWin32::RawOpen(const std::string &fileName, FileOpenMode mode)
	{
		return std::make_shared<OSFile>(_rootDirectory + L'\\' + s2w(fileName), mode);
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

			// tmpDir = _rootDirectory + '\\' + dirName
			std::wstring tmpDir = _rootDirectory + L"\\";
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

			std::shared_ptr<IFileSystem> child = std::make_shared<FileSystemWin32>(_rootDirectory + L'\\' + s2w(dirName));
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
				return std::make_shared<FileSystemWin32>(std::move(tmpFull));
			}
		}
		throw std::runtime_error(StrFromErr(GetLastError()));
		return nullptr;
	}
}