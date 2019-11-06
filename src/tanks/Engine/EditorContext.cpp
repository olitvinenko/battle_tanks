#include "EditorContext.h"
#include "Pathfinder.h"
#include "FileSystem.h"
#include "World.h"

EditorContext::EditorContext(std::shared_ptr<FileSystem::File> stream, TextureManager& texManager)
	: m_pathfinder(new Pathfinder(texManager))
{
	m_pathfinder->Load(stream->AsSTDStream());
    
    int width = m_pathfinder->GetSizeX() * 32;
    int height = m_pathfinder->GetSizeY() * 32;
    
    RectInt bounds{ -width / 2, -height / 2, width / 2, height / 2 };
    
    _originalBounds = MakeRectWH(Vector2(0,0), Vector2(m_pathfinder->GetSizeX(), m_pathfinder->GetSizeY()));
    
    bounds.left = std::min(bounds.left, 0);
    bounds.top = std::min(bounds.top, 0);
    bounds.right = std::max(bounds.right, 32);
    bounds.bottom = std::max(bounds.bottom, 32);
    
    _world.reset(new World(bounds));
}

EditorContext::EditorContext(int width, int height, TextureManager& texManager)
	: m_pathfinder(new Pathfinder(texManager))
{
	m_pathfinder->CreateGraph(width, height);
    
    RectInt bounds{ -width / 2, -height / 2, width / 2, height / 2 };
    _world.reset(new World(bounds));
}

void EditorContext::FixedUpdate(float dt)
{
}
