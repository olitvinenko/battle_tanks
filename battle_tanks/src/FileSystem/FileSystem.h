#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

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
		Begin, Current, End
	};

	FileOpenMode operator |(FileOpenMode lhs, FileOpenMode rhs);
	FileOpenMode operator &(FileOpenMode lhs, FileOpenMode rhs);

	struct IMemoryMap
	{
		virtual char* GetData() = 0;
		virtual unsigned long GetSize() const = 0;
		virtual void SetSize(unsigned long size) = 0;
	};

	struct IStream
	{
		virtual size_t Read(void *dst, size_t size, size_t count) = 0;
		virtual void Write(const void *src, size_t size) = 0;
		virtual void Seek(long long amount, SeekMethod method) = 0;
		virtual long long Tell() const = 0;
	};

	struct IFile
	{
		virtual std::shared_ptr<IMemoryMap> QueryMap() = 0;
		virtual std::shared_ptr<IStream> QueryStream() = 0;
	};

	class IFileSystem
	{
	public:
		virtual std::shared_ptr<IFileSystem> GetFileSystem(const std::string &path, bool create = false, bool nothrow = false);
		virtual std::vector<std::string> EnumAllFiles(const std::string &mask) = 0;

		std::shared_ptr<IFile> Open(const std::string &path, FileOpenMode mode = FileOpenMode::Read);
		void Mount(const std::string &nodeName, std::shared_ptr<IFileSystem> fs);

	protected:
		virtual std::shared_ptr<IFile> RawOpen(const std::string &fileName, FileOpenMode mode) = 0; // open a file that strictly belongs to this file system

	private:
		std::unordered_map<std::string, std::shared_ptr<IFileSystem>> _children;
	};

	std::shared_ptr<IFileSystem> CreateOSFileSystem(const std::string &rootDirectory);
}