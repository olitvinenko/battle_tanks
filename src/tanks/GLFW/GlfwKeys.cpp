#include "GlfwKeys.h"
#include "Keys.h"
#include <glfw/glfw3.h>

UI::Key MapGlfwKeyCode(int platformKey)
{
	switch(platformKey)
	{
#define GEN_KEY_ENTRY(platformKey, uiKey) case platformKey: return uiKey;
#include "GlfwKeys.gen"
#undef GEN_KEY_ENTRY
		default:
			break;
	}

	return UI::Key::Unknown;
}

int UnmapGlfwKeyCode(UI::Key key)
{
	switch(key)
	{
#define GEN_KEY_ENTRY(platformKey, uiKey) case uiKey: return platformKey;
#include "GlfwKeys.gen"
#undef GEN_KEY_ENTRY
		default:
			break;
	}

	return -1;
}
