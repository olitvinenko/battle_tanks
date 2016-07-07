
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windef.h>
#include <iostream>

int APIENTRY WinMain(HINSTANCE, // hInstance
	HINSTANCE, // hPrevInstance
	LPSTR, // lpCmdLine
	int) // nCmdShow
#else
int main(int, const char**)
#endif
{
	std::cout << "Hello world!" << std::endl;
}