#include "World.h"
#include "WorldCfg.h"
#include "FileSystem.h"
#include <cassert>

// don't create game objects in the constructor
World::World(int X, int Y)
  : _gameStarted(false)
  , _frozen(false)
  , _nightMode(false)
  , _sx(0)
  , _sy(0)
  , _locationsX(0)
  , _locationsY(0)
  , _seed(1)
  , _safeMode(true)
  , _time(0)
{
	_locationsX  = (X * CELL_SIZE / LOCATION_SIZE + ((X * CELL_SIZE) % LOCATION_SIZE != 0 ? 1 : 0));
	_locationsY  = (Y * CELL_SIZE / LOCATION_SIZE + ((Y * CELL_SIZE) % LOCATION_SIZE != 0 ? 1 : 0));
	_sx          = (float) X * CELL_SIZE;
	_sy          = (float) Y * CELL_SIZE;
	_cellsX      = X;
	_cellsY      = Y;

	_waterTiles.resize(X * Y);
	_woodTiles.resize(X * Y);
}

World::~World()
{
}

void World::Step(float dt)
{
	
}


///////////////////////////////////////////////////////////////////////////////
// end of file
