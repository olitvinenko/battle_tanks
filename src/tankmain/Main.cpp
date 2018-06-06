#include <cstdlib>
#include <ctime>
#include <algorithm>

//fs tests
#include <memory>
#include <vector>
#include <iostream>

#include <FileSystem.h>
#include <Platform.h> //TODO::


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windef.h>

#include <string>
#include <stdio.h>  // FILENAME_MAX
#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string CurrentWorkingDirectory()
{
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		return std::string();
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	return std::string(cCurrentPath);
}


int APIENTRY WinMain( HINSTANCE, // hInstance
                      HINSTANCE, // hPrevInstance
                      LPSTR, // lpCmdLine
                      int) // nCmdShow
#else
int main(int, const char**)
#endif
{
	try
	{
		srand((unsigned int)time(nullptr));

#if defined(_DEBUG) && defined(_WIN32) // memory leaks detection
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		std::string path = CurrentWorkingDirectory();

		std::shared_ptr<FileSystem::IFileSystem> fs = FileSystem::CreateOSFileSystem(path);
		std::vector<std::string> files = fs->EnumAllFiles("*.txt");

		for (auto it = files.begin(); it != files.end(); ++it)
			std::cout << (*it) << std::endl;

		auto inner = fs->GetFileSystem("Dir1", true);

		//auto file = inner->Open("test.txt");
		//auto stream = file->QueryStream();
		//
		//char* ch = new char[11];
		//ch[10] = '\0';
		//auto s = stream->Read(ch, sizeof(char), 10);
		//std::cout << ch << std::endl;
		//delete[] ch;

		//stream->Seek(0, FileSystem::SeekMethod::End);
		//std::cout << stream->Tell() << std::endl;

		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;	
		return 1;
	}
}