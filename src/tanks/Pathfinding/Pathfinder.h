#pragma once

#include <vector>
#include <list>

#include "GraphNodeTypes.h"
#include "GraphEdgeTypes.h"
#include "SparseGraph.h"

class DrawingContext;
class TextureManager;
class Vector2;

class Pathfinder final
{
public:

	enum brush_type
	{
		unknown = -1,

		normal = 0,
		obstacle = 1,
		water = 2,
		mud = 3,
		source = 4,
		target = 5,

		count
	};

	enum algorithm_type
	{
		non,

		search_dfs,
		search_bfs,
		search_dijkstra,
		search_astar,

		max
	};

private:

	//the terrain type of each cell
	std::vector<int>              m_TerrainType;

	//this vector will store any path returned from a graph search
	std::list<int>                m_Path;

	//create a typedef for the graph type
	typedef SparseGraph<NavGraphNode<void*>, GraphEdge> NavGraph;

	NavGraph*                     m_pGraph;

	//this vector of edges is used to store any subtree returned from 
	//any of the graph algorithms (such as an SPT)
	std::vector<const GraphEdge*> m_SubTree;

	//the total cost of the path from target to source
	double                         m_dCostToTarget;

	//the currently selected algorithm
	algorithm_type                m_CurrentAlgorithm;

	//the current terrain brush
	brush_type                    m_CurrentTerrainBrush;

	//the dimensions of the cells
	float                        m_dCellWidth;
	float                        m_dCellHeight;

	//number of cells vertically and horizontally
	int                           m_iCellsX,
		m_iCellsY;

	//local record of the client area
	int                           m_icxClient,
		m_icyClient;

	//the indices of the source and target cells
	int                           m_iSourceCell,
		m_iTargetCell;

	//flags to indicate if the start and finish points have been added
	bool                          m_bStart,
		m_bFinish;

	//should the graph (nodes and GraphEdges) be rendered?
	bool                          m_bShowGraph;

	//should the tile outlines be rendered
	bool                          m_bShowTiles;

	//this calls the appropriate algorithm
	void  UpdateAlgorithm();

	//helper function for PaintTerrain (see below)
	void  UpdateGraphFromBrush(int brush, int CellIndex);

	std::string GetNameOfCurrentSearchAlgorithm()const;

	size_t m_backTexture;
	size_t m_obstacleTexture;
	size_t m_waterTexture;
	size_t m_mudTexture;

	size_t m_lineTexture;

	size_t m_startPointTexture;
	size_t m_finishPointTexture;
	
public:

	explicit Pathfinder(const TextureManager& tm);
	~Pathfinder() { delete m_pGraph; }

	void CreateGraph(int x, int y);

	void Render(DrawingContext& dc);

	int GetSizeX() const { return m_icxClient; }
	int GetSizeY() const { return m_icyClient; }

	//this will paint whatever cell the cursor is currently over in the 
	//currently selected terrain brush
	void PaintTerrain(const Vector2& point);
	void ChangeBrush(const brush_type NewBrush) { m_CurrentTerrainBrush = NewBrush; }

	//the algorithms
	void CreatePathDFS();
	void CreatePathBFS();
	void CreatePathDijkstra();
	void CreatePathAStar();

	void ToggleShowGraph(bool show) { m_bShowGraph = show; }
	void ToggleShowTiles(bool show) { m_bShowTiles = show; }

	void ChangeSource(const int cell) { m_iSourceCell = cell; }
	void ChangeTarget(const int cell) { m_iTargetCell = cell; }

	//converts a POINTS to an index into the graph. Returns false if p
	//is invalid
	bool PointToIndex(const Vector2& point, int& NodeIndex);
	brush_type GetTileType(const Vector2& point);

	//returns the terrain cost of the brush type
	float GetTerrainCost(brush_type brush);

	void Save(std::fstream& fs);
	void Load(std::fstream& fs);
};
