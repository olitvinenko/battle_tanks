#include "EditorContext.h"
#include "Pathfinder.h"
#include "FileSystem.h"

EditorContext::EditorContext(std::shared_ptr<FileSystem::File> stream, TextureManager& texManager)
	: m_pathfinder(new Pathfinder(texManager))
{
	m_pathfinder->Load(stream->AsSTDStream());
}

EditorContext::EditorContext(int width, int height, TextureManager& texManager)
	: m_pathfinder(new Pathfinder(texManager))
{
	m_pathfinder->CreateGraph(width, height);
}

void EditorContext::FixedUpdate(float dt)
{
}
