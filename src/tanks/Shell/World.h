#pragma once
#include <memory>
#include <set>
#include <vector>


#define SAFE_CANCEL(ro) if(ro) { ro->Cancel(); ro = nullptr; } else (void)0

class World
{
	World(const World&) = delete;
	World& operator=(const World&) = delete;
public:

	static const unsigned int NET_RAND_MAX = 0xffff;

	std::vector<bool> _waterTiles;
	std::vector<bool> _woodTiles;

	bool    _gameStarted;
	bool    _frozen;
	bool    _nightMode;
	float   _sx, _sy;   // world size
	int     _cellsX;
	int     _cellsY;

	int _locationsX;
	int _locationsY;

	std::string _infoAuthor;
	std::string _infoEmail;
	std::string _infoUrl;
	std::string _infoDesc;
	std::string _infoTheme;
	std::string _infoOnInit;

	unsigned long _seed;

	bool  _safeMode;

public:

	World(int X, int Y);
	~World();

	void Step(float dt);


	float _time;
};

