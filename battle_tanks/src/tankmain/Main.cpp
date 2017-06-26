#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windef.h>
int APIENTRY WinMain( HINSTANCE, // hInstance
                      HINSTANCE, // hPrevInstance
                      LPSTR, // lpCmdLine
                      int) // nCmdShow
#else
int main(int, const char**)
#endif
try
{
	srand((unsigned int) time(nullptr));

#if defined(_DEBUG) && defined(_WIN32) // memory leaks detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	

	return 0;
}
catch (const std::exception &e)
{
	return 1;
}


