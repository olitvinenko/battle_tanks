#include "GlfwKeys.h"
#include "Keys.h"

#include <GLFW/glfw3.h>

Key MapGlfwKeyCode(int platformKey)
{
	switch(platformKey)
	{
#define GEN_KEY_ENTRY(platformKey, uiKey) case platformKey: return uiKey;
#include "GlfwKeys.gen"
#undef GEN_KEY_ENTRY
		default:
			break;
	}

	return Key::Unknown;
}

int UnmapGlfwKeyCode(Key key)
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
