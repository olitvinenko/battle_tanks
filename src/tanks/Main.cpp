#include <cstdlib>
#include <ctime>
#include <algorithm>

//fs tests
#include <memory>
#include <vector>
#include <iostream>
#include <string>

#include <FileSystem.h>
#include <Platform.h> //TODO::


//int APIENTRY WinMain( HINSTANCE, // hInstance
//                      HINSTANCE, // hPrevInstance
//                      LPSTR, // lpCmdLine
//                      int) // nCmdShow
//#else
int main(int, const char**)
//#endif
{
	try
	{
		srand((unsigned int)time(nullptr));

#if defined(_DEBUG) && defined(_WIN32) // memory leaks detection
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		std::shared_ptr<FileSystem::IFileSystem> fs = FileSystem::CreateOSFileSystem("data");
		std::vector<std::string> files = fs->EnumAllFiles("*.txt");

		for (auto it = files.begin(); it != files.end(); ++it)
			std::cout << *it << std::endl;

		//auto inner = fs->GetFileSystem("Dir1", true);

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