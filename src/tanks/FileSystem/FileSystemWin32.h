#pragma once

#include "FileSystem.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace FileSystem
{
	class FileSystemWin32 : public IFileSystem
	{
	protected:
		std::shared_ptr<IFile> RawOpen(const std::string &fileName, FileOpenMode mode) override;

	public:
		explicit FileSystemWin32(std::wstring &&rootDirectory);

		std::shared_ptr<IFileSystem> GetFileSystem(const std::string &path, bool create = false, bool nothrow = false) override;
		std::vector<std::string> EnumAllFiles(const std::string &mask) override;

	public:
		struct AutoHandle
		{
			HANDLE h;
			AutoHandle() : h(nullptr) {}
			~AutoHandle()
			{
				if (nullptr != h && INVALID_HANDLE_VALUE != h)
				{
					CloseHandle(h);
				}
			}
		private:
			AutoHandle(const AutoHandle&) = delete;
			AutoHandle& operator = (const AutoHandle&) = delete;
		};

		class OSFile
			: public IFile
			, public std::enable_shared_from_this<OSFile>
		{
		public:
			OSFile(std::wstring &&fileName, FileOpenMode mode);
			virtual ~OSFile();

			// File
			std::shared_ptr<IMemoryMap> QueryMap() override;
			std::shared_ptr<IStream> QueryStream() override;
			void Unmap();
			void Unstream();

		private:
			class OSMemMap : public IMemoryMap
			{
			public:
				OSMemMap(std::shared_ptr<OSFile> parent, HANDLE hFile);
				virtual ~OSMemMap();

				virtual char* GetData();
				virtual unsigned long GetSize() const;
				virtual void SetSize(unsigned long size); // may invalidate pointer returned by GetData()

			private:
				std::shared_ptr<OSFile> _file;
				HANDLE _hFile;
				AutoHandle _map;
				void *_data;
				size_t _size;
				void SetupMapping();
			};

			class OSStream : public IStream
			{
			public:
				OSStream(std::shared_ptr<OSFile> parent, HANDLE hFile);
				virtual ~OSStream();

				virtual size_t Read(void *dst, size_t size, size_t count);
				virtual void Write(const void *src, size_t size);
				virtual void Seek(long long amount, SeekMethod method);
				virtual long long Tell() const;

			private:
				std::shared_ptr<OSFile> _file;
				HANDLE _hFile;
			};

		private:
			AutoHandle _file;
			FileOpenMode _mode;
			bool _mapped;
			bool _streamed;
		};

		std::wstring _rootDirectory;
	};
}
