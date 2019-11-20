#include "FileSystem.h"
#include <cassert>

namespace FileSystem
{
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

	static int toCppMode(FileOpenMode mode)
	{
		int result = 0;

		if ((mode & FileOpenMode::Read) == FileOpenMode::Read)
			result |= std::ios::in;

		if ((mode & FileOpenMode::Write) == FileOpenMode::Write)
			result |= std::ios::out;

		if ((mode & FileOpenMode::Append) == FileOpenMode::Append)
			result |= std::ios::app;

		if ((mode & FileOpenMode::Binary) == FileOpenMode::Binary)
			result |= std::ios::binary;

		if ((mode & FileOpenMode::AtEnd) == FileOpenMode::AtEnd)
			result |= std::ios::ate;

		if ((mode & FileOpenMode::Truncate) == FileOpenMode::Truncate)
			result |= std::ios::trunc;

		return result;
	}

	static std::ios_base::seek_dir toCppSeek(SeekMethod method)
	{
		switch (method)
		{
		case SeekMethod::Begin:
			return std::ios_base::beg;
		case SeekMethod::Current:
			return std::ios_base::cur;
		case SeekMethod::End:
			return std::ios_base::end;
                
        default: assert(false && "Not supported seek method");
		}
	}


	File::File(const std::string& path, FileOpenMode mode)
		: _file(path, toCppMode(mode))
		, _mode(mode)
	{
    }

	File::~File()
	{
		_file.close();
	}

	std::shared_ptr<Memory> File::AsMemory()
	{
		assert(!_mapped && !_streamed);
		_mapped = true;
		return std::make_shared<Memory>(shared_from_this());
	}

	std::shared_ptr<Stream> File::AsStream()
	{
		assert(!_mapped && !_streamed);
		_streamed = true;
		return std::make_shared<Stream>(shared_from_this());
	}

	std::fstream& File::AsSTDStream()
	{
		return _file;
	}

	void File::Unmap()
	{
		assert(_mapped && !_streamed);
		_mapped = false;
	}

	void File::Unstream()
	{
		assert(_streamed && !_mapped);
		_streamed = false;
	}


	Memory::Memory(std::shared_ptr<File> file)
		: _parent(file)
		, _data(nullptr)
		, _size(0)
	{
		if (!_parent->_file.is_open())
			return;

		_parent->_file.seekg(0, std::ios::end);
		_size = static_cast<unsigned long>(_parent->_file.tellg());

		_parent->_file.seekg(0, std::ios::beg);
		_data = new char[_size];
		_parent->_file.read(_data, _size);
		_parent->_file.seekg(0, std::ios::beg);
	}

	Memory::~Memory()
	{
		if (_data)
			delete[] _data;

		_parent->Unmap();
	}

	char* Memory::GetData() const
	{
		return _data;
	}

	unsigned long Memory::GetSize() const
	{
		return _size;
	}

	Stream::Stream(std::shared_ptr<File> file)
		: _parent(file)
	{
		SeekPut(0, SeekMethod::Begin);
		SeekGet(0, SeekMethod::Begin);
	}

	Stream::~Stream()
	{
		_parent->Unstream();
	}

	size_t Stream::Read(void *dst, size_t size, size_t count) const
	{
		_parent->_file.read((char*)dst, size * count);
		return static_cast<size_t>(_parent->_file.gcount()) / size;
	}

	void Stream::Write(const void *src, size_t size) const
	{
		_parent->_file.write((const char*)src, size);
	}

	void Stream::SeekGet(long long amount, SeekMethod method) const
	{
		_parent->_file.seekg(amount, toCppSeek(method));
	}

    void Stream::SeekGet(long long amount) const
    {
        _parent->_file.seekg(amount);
    }

	void Stream::SeekPut(long long amount, SeekMethod method) const
	{
		_parent->_file.seekp(amount, toCppSeek(method));
	}

    std::string Stream::GetContent() const
    {
        SeekGet(0, SeekMethod::End);
        size_t size = TellGet();
        std::string buffer(size, ' ');
        SeekGet(0);
        Read(&buffer[0], sizeof(char), size);
        SeekGet(0, SeekMethod::Begin);
        return buffer;
    }

	long long Stream::TellGet() const
	{
		return _parent->_file.tellg();
	}

	long long Stream::TellPut() const
	{
		return _parent->_file.tellp();
	}

	IFileSystem::IFileSystem(const std::string& rootDirectory)
		: _rootDirectory(rootDirectory)
	{ }

	void IFileSystem::Mount(const std::string &nodeName, std::shared_ptr<IFileSystem> fs)
	{
		assert(!nodeName.empty() && std::string::npos == nodeName.find('/'));
		_children[nodeName] = fs;
	}

	std::shared_ptr<File> IFileSystem::Open(const std::string &fileName, FileOpenMode mode)
	{
		std::string::size_type pd = fileName.rfind('/');
		if (pd && std::string::npos != pd) // was a path delimiter found?
		{
			return GetFileSystem(fileName.substr(0, pd))->RawOpen(fileName.substr(pd + 1), mode);
		}
		return RawOpen(fileName, mode);
	}

	// open a file that strictly belongs to this file system
	std::shared_ptr<File> IFileSystem::RawOpen(const std::string &fileName, FileOpenMode mode) const
	{
		return std::make_shared<File>(_rootDirectory + Separator() + fileName, mode);
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
