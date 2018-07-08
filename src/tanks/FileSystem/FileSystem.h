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

	class File : public std::enable_shared_from_this<File>
	{
	public:
		File(const std::string& path, FileOpenMode mode);
		~File();

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

		class Stream
		{
		public:
			explicit Stream(std::shared_ptr<File> file);
			~Stream();

			size_t Read(void *dst, size_t size, size_t count) const;
			void Write(const void *src, size_t size) const;

			void SeekGet(long long amount, SeekMethod method) const;
			void SeekPut(long long amount, SeekMethod method) const;

			long long TellGet() const;
			long long TellPut() const;

		private:
			std::shared_ptr<File> _parent;
		};
		
		std::shared_ptr<Memory> AsMemory();
		std::shared_ptr<Stream> AsStream();

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

		std::shared_ptr<File> Open(const std::string &path, FileOpenMode mode = FileOpenMode::Read | FileOpenMode::Binary);
		void Mount(const std::string &nodeName, std::shared_ptr<IFileSystem> fs);

		virtual std::vector<std::string> EnumAllFiles(const std::string &mask) = 0;
		virtual char Separator() const = 0;

	protected:
		explicit IFileSystem(std::string &&rootDirectory);
		
		std::string _rootDirectory;
	private:
		std::shared_ptr<File> RawOpen(const std::string &fileName, FileOpenMode mode) const;
		std::unordered_map<std::string, std::shared_ptr<IFileSystem>> _children;
	};

	std::shared_ptr<IFileSystem> CreateOSFileSystem(const std::string &rootDirectory);
}