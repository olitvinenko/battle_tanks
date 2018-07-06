#include "EditorContext.h"
#include "World.h"
#include <stdexcept>

EditorContext::EditorContext(FileSystem::IStream &stream)
{
	//MapFile mf(stream, false);

	//int width = 0;
	//int height = 0;
	//if (!mf.getMapAttribute("width", width) ||
	//	!mf.getMapAttribute("height", height))
	//{
	//	throw std::runtime_error("unknown map size");
	//}

	//_world.reset(new World(width, height));
	//_world->Import(mf);
}

EditorContext::EditorContext(int width, int height)
	: _world(new World(width, height))
{
}


void EditorContext::FixedUpdate(float dt)
{
	_world->Step(dt);
}
