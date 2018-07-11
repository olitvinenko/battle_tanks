#include "EditorContext.h"
#include <stdexcept>


EditorContext::EditorContext(std::shared_ptr<FileSystem::File> stream, TextureManager& texManager)
	: m_game(new Raven_Game(texManager))
{
	m_game->LoadMap(stream->AsSTDStream());

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

EditorContext::EditorContext(int width, int height, TextureManager& texManager)
	: m_game(new Raven_Game(width, height, texManager))
{
}


void EditorContext::FixedUpdate(float dt)
{
	m_game->Update();
}
