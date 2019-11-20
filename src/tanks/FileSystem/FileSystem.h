#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <fstream>
#include <cassert>

namespace FileSystem
{
	enum class FileOpenMode
	{
		Read = 1u << 0,
		Write = 1u << 1,
		Append = 1u << 2,
		Binary = 1u << 3,
		AtEnd = 1u << 4,
		Truncate = 1u << 5,
	};

	enum class SeekMethod
	{
		Begin,
		Current,
		End
	};

	FileOpenMode operator |(FileOpenMode lhs, FileOpenMode rhs);
	FileOpenMode operator &(FileOpenMode lhs, FileOpenMode rhs);

    class File;

    class Stream
    {
    public:
        explicit Stream(std::shared_ptr<File> file);
        ~Stream();

        size_t Read(void *dst, size_t size, size_t count) const;
        void Write(const void *src, size_t size) const;

        void SeekGet(long long amount, SeekMethod method) const;
        void SeekGet(long long amount) const;
        void SeekPut(long long amount, SeekMethod method) const;
        
        std::string GetContent() const;

        long long TellGet() const;
        long long TellPut() const;
    private:
        std::shared_ptr<File> _parent;
    };

    class Memory
    {
    public:
        explicit Memory(std::shared_ptr<File> file);
        ~Memory();

        char* GetData() const;
        unsigned long GetSize() const;

    private:
        char* _data;
        unsigned long _size;
        std::shared_ptr<File> _parent;
    };

	class File : public std::enable_shared_from_this<File>
	{
        friend class Memory;
        friend class Stream;
	public:
		File(const std::string& path, FileOpenMode mode);
		~File();
		
		std::shared_ptr<Memory> AsMemory();
		std::shared_ptr<Stream> AsStream();

		std::fstream& AsSTDStream();

	private:
		void Unmap();	
		void Unstream();	

		bool _streamed = false;
		bool _mapped = false;

		std::fstream _file;
		FileOpenMode _mode;
	};

	class IFileSystem
	{
	public:
		virtual ~IFileSystem() = default;

		virtual std::shared_ptr<IFileSystem> GetFileSystem(const std::string &path, bool create = false, bool nothrow = false);
        virtual std::vector<std::string> EnumAllFiles(const std::string &mask) = 0;
        
		std::shared_ptr<File> Open(const std::string &path, FileOpenMode mode = FileOpenMode::Read | FileOpenMode::Binary);
		void Mount(const std::string &nodeName, std::shared_ptr<IFileSystem> fs);
        
        const std::string& GetRootDirectory() const { return _rootDirectory; }
	protected:
		explicit IFileSystem(const std::string& rootDirectory);
    
        virtual char Separator() const = 0;
		std::string _rootDirectory;
	private:
		std::shared_ptr<File> RawOpen(const std::string &fileName, FileOpenMode mode) const;
		std::unordered_map<std::string, std::shared_ptr<IFileSystem>> _children;
	};

	std::shared_ptr<IFileSystem> CreateOSFileSystem(const std::string &rootDirectory);
}
