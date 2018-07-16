#include "Pathfinder.h"
#include "HandyGraphFunctions.h"
#include "DrawingContext.h"
#include "WorldCfg.h"
#include "TextureManager.h"
#include "Vector2.h"

Pathfinder::Pathfinder(const TextureManager& tm)
	:m_bStart(false),
	m_bFinish(false),
	m_bShowGraph(false),
	m_bShowTiles(false),
	m_dCellWidth(0),
	m_dCellHeight(0),
	m_iCellsX(0),
	m_iCellsY(0),
	m_CurrentTerrainBrush(normal),
	m_iSourceCell(0),
	m_iTargetCell(0),
	m_icxClient(0),
	m_icyClient(0),
	m_dCostToTarget(0.0),
	m_pGraph(nullptr),

	m_backTexture(tm.FindSprite("background")),
	m_lineTexture(tm.FindSprite("lightning")),
	m_obstacleTexture(tm.FindSprite("concrete_wall")),
	m_waterTexture(tm.FindSprite("water")),
	m_mudTexture(tm.FindSprite("brick_wall")),
	m_startPointTexture(tm.FindSprite("editor_item")),
	m_finishPointTexture(tm.FindSprite("editor_respawn"))
{
}

//----------------------- CreateGraph ------------------------------------
//
//------------------------------------------------------------------------
void Pathfinder::CreateGraph(int x, int y)
{
	m_icxClient = y * CELL_SIZE;
	m_icyClient = x * CELL_SIZE;

	//initialize the terrain vector with normal terrain
	m_TerrainType.assign(x *  y, normal);

	m_iCellsX = y;
	m_iCellsY = x;
	m_dCellWidth = (double)m_icxClient / (double)y;
	m_dCellHeight = (double)m_icyClient / (double)x;

	//delete any old graph
	delete m_pGraph;

	//create the graph
	m_pGraph = new NavGraph(false);//not a digraph

	GraphHelper_CreateGrid(*m_pGraph, m_icxClient, m_icyClient, x, y);

	//initialize source and target indexes to mid top and bottom of grid 
	PointToIndex(VectorToPOINTS(Vector2D(m_icxClient / 2, m_dCellHeight * 2)), m_iTargetCell);
	PointToIndex(VectorToPOINTS(Vector2D(m_icxClient / 2, m_icyClient - m_dCellHeight * 2)), m_iSourceCell);

	m_Path.clear();
	m_SubTree.clear();

	m_CurrentAlgorithm = non;
}

//--------------------- PointToIndex -------------------------------------
//
//  converts a POINTS into an index into the graph
//------------------------------------------------------------------------
bool Pathfinder::PointToIndex(POINTS p, int& NodeIndex)
{
	//convert p to an index into the graph
	int x = (int)((double)(p.x) / m_dCellWidth);
	int y = (int)((double)(p.y) / m_dCellHeight);

	//make sure the values are legal
	if ((x>m_iCellsX) || (y>m_iCellsY))
	{
		NodeIndex = -1;

		return false;
	}

	NodeIndex = y*m_iCellsX + x;

	return true;
}

//----------------- GetTerrainCost ---------------------------------------
//
//  returns the cost of the terrain represented by the current brush type
//------------------------------------------------------------------------
double Pathfinder::GetTerrainCost(const brush_type brush)
{
	const double cost_normal = 1.0;
	const double cost_water = 2.0;
	const double cost_mud = 1.5;

	switch (brush)
	{
	case normal: return cost_normal;
	case water:  return cost_water;
	case mud:    return cost_mud;
	default:     return MaxDouble;
	};
}

//----------------------- PaintTerrain -----------------------------------
//
//  this either changes the terrain at position p to whatever the current
//  terrain brush is set to, or it adjusts the source/target cell
//------------------------------------------------------------------------
void Pathfinder::PaintTerrain(const math::Vector2& point)
{
	//convert p to an index into the graph
	int x = (int)((double)(point.x) / m_dCellWidth);
	int y = (int)((double)(point.y) / m_dCellHeight);

	//make sure the values are legal
	if ((x>m_iCellsX) || (y>(m_iCellsY - 1))) return;

	//reset path and tree records
	m_SubTree.clear();
	m_Path.clear();

	//if the current terrain brush is set to either source or target we
	//should change the appropriate node
	if ((m_CurrentTerrainBrush == source) || (m_CurrentTerrainBrush == target))
	{
		switch (m_CurrentTerrainBrush)
		{
		case source:

			m_iSourceCell = y*m_iCellsX + x; break;

		case target:

			m_iTargetCell = y*m_iCellsX + x; break;

		}//end switch
	}

	//otherwise, change the terrain at the current mouse position
	else
	{
		UpdateGraphFromBrush(m_CurrentTerrainBrush, y*m_iCellsX + x);
	}

	//update any currently selected algorithm
	UpdateAlgorithm();
}

//--------------------------- UpdateGraphFromBrush ----------------------------
//
//  given a brush and a node index, this method updates the graph appropriately
//  (by removing/adding nodes or changing the costs of the node's edges)
//-----------------------------------------------------------------------------
void Pathfinder::UpdateGraphFromBrush(int brush, int CellIndex)
{
	//set the terrain type in the terrain index
	m_TerrainType[CellIndex] = brush;

	//if current brush is an obstacle then this node must be removed
	//from the graph
	if (brush == 1)
	{
		m_pGraph->RemoveNode(CellIndex);
	}

	else
	{
		//make the node active again if it is currently inactive
		if (!m_pGraph->isNodePresent(CellIndex))
		{
			int y = CellIndex / m_iCellsY;
			int x = CellIndex - (y*m_iCellsY);

			m_pGraph->AddNode(NavGraph::NodeType(CellIndex, Vector2D(x*m_dCellWidth + m_dCellWidth / 2.0,
				y*m_dCellHeight + m_dCellHeight / 2.0)));

			GraphHelper_AddAllNeighboursToGridNode(*m_pGraph, y, x, m_iCellsX, m_iCellsY);
		}

		//set the edge costs in the graph
		WeightNavGraphNodeEdges(*m_pGraph, CellIndex, GetTerrainCost((brush_type)brush));
	}
}

//--------------------------- UpdateAlgorithm ---------------------------------
void Pathfinder::UpdateAlgorithm()
{
	//update any current algorithm
	switch (m_CurrentAlgorithm)
	{
	case non:

		break;

	case search_dfs:

		CreatePathDFS(); break;

	case search_bfs:

		CreatePathBFS(); break;

	case search_dijkstra:

		CreatePathDijkstra(); break;

	case search_astar:

		CreatePathAStar(); break;

	default: break;
	}
}

//------------------------- CreatePathDFS --------------------------------
//
//  uses DFS to find a path between the start and target cells.
//  Stores the path as a series of node indexes in m_Path.
//------------------------------------------------------------------------
void Pathfinder::CreatePathDFS()
{
	//set current algorithm
	m_CurrentAlgorithm = search_dfs;

	//clear any existing path
	m_Path.clear();
	m_SubTree.clear();

	//do the search
	Graph_SearchDFS<NavGraph> DFS(*m_pGraph, m_iSourceCell, m_iTargetCell);

	//now grab the path (if one has been found)
	if (DFS.Found())
	{
		m_Path = DFS.GetPathToTarget();
	}

	m_SubTree = DFS.GetSearchTree();

	m_dCostToTarget = 0.0;
}


//------------------------- CreatePathBFS --------------------------------
//
//  uses BFS to find a path between the start and target cells.
//  Stores the path as a series of node indexes in m_Path.
//------------------------------------------------------------------------
void Pathfinder::CreatePathBFS()
{
	//set current algorithm
	m_CurrentAlgorithm = search_bfs;

	//clear any existing path
	m_Path.clear();
	m_SubTree.clear();

	//do the search
	Graph_SearchBFS<NavGraph> BFS(*m_pGraph, m_iSourceCell, m_iTargetCell);

	//now grab the path (if one has been found)
	if (BFS.Found())
	{
		m_Path = BFS.GetPathToTarget();
	}

	m_SubTree = BFS.GetSearchTree();

	m_dCostToTarget = 0.0;
}

//-------------------------- CreatePathDijkstra --------------------------
//
//  creates a path from m_iSourceCell to m_iTargetCell using Dijkstra's algorithm
//------------------------------------------------------------------------
void Pathfinder::CreatePathDijkstra()
{
	//set current algorithm
	m_CurrentAlgorithm = search_dijkstra;

	//create and start a timer
	Graph_SearchDijkstra<NavGraph> djk(*m_pGraph, m_iSourceCell, m_iTargetCell);

	m_Path = djk.GetPathToTarget();

	m_SubTree = djk.GetSPT();

	m_dCostToTarget = djk.GetCostToTarget();
}

//--------------------------- CreatePathAStar ---------------------------
//------------------------------------------------------------------------
void Pathfinder::CreatePathAStar()
{
	//set current algorithm
	m_CurrentAlgorithm = search_astar;

	//create a couple of typedefs so the code will sit comfortably on the page   
	typedef Graph_SearchAStar<NavGraph, Heuristic_Euclid> AStarSearch;

	//create an instance of the A* search using the Euclidean heuristic
	AStarSearch AStar(*m_pGraph, m_iSourceCell, m_iTargetCell);

	m_Path = AStar.GetPathToTarget();

	m_SubTree = AStar.GetSPT();

	m_dCostToTarget = AStar.GetCostToTarget();

}

//---------------------------Load n save methods ------------------------------
//-----------------------------------------------------------------------------
void Pathfinder::Save(std::fstream& fs)
{
	assert(fs && "Pathfinder::Save< bad file >");

	//save the size of the grid
	fs << m_iCellsX << std::endl;
	fs << m_iCellsY << std::endl;

	//save the terrain
	for (unsigned int t = 0; t<m_TerrainType.size(); ++t)
	{
		if (t == m_iSourceCell)
		{
			fs << source << std::endl;
		}
		else if (t == m_iTargetCell)
		{
			fs << target << std::endl;
		}
		else
		{
			fs << m_TerrainType[t] << std::endl;
		}
	}
}

//-------------------------------- Load ---------------------------------------
//-----------------------------------------------------------------------------
void Pathfinder::Load(std::fstream& fs)
{
	assert(fs && "Pathfinder::Save< bad file >");

	//load the size of the grid
	fs >> m_iCellsX;
	fs >> m_iCellsY;

	//create a graph of the correct size
	CreateGraph(m_iCellsY, m_iCellsX);

	int terrain;

	//save the terrain
	for (int t = 0; t<m_iCellsX*m_iCellsY; ++t)
	{
		fs >> terrain;

		if (terrain == source)
		{
			m_iSourceCell = t;
		}

		else if (terrain == target)
		{
			m_iTargetCell = t;
		}

		else
		{
			m_TerrainType[t] = terrain;

			UpdateGraphFromBrush(terrain, t);
		}
	}
}

//------------------------ GetNameOfCurrentSearchAlgorithm --------------------
//-----------------------------------------------------------------------------
std::string Pathfinder::GetNameOfCurrentSearchAlgorithm()const
{
	switch (m_CurrentAlgorithm)
	{
	case non: return "";
	case search_astar: return "A Star";
	case search_bfs: return "Breadth First";
	case search_dfs: return "Depth First";
	case search_dijkstra: return "Dijkstras";
	default: return "UNKNOWN!";
	}
}

//---------------------------- Render ------------------------------------
//
//------------------------------------------------------------------------
void Pathfinder::Render(DrawingContext& dc)
{
	dc.DrawBackground(m_backTexture, m_icxClient, m_icyClient);

	Vector2 dir(1, 0);
	//render all the cells
	for (int nd = 0; nd<m_pGraph->NumNodes(); ++nd)
	{
		auto pos = m_pGraph->GetNode(nd).Pos();
		int left = (int)(pos.x - m_dCellWidth / 2.0);
		int top = (int)(pos.y - m_dCellHeight / 2.0);
		int right = (int)(1 + pos.x + m_dCellWidth / 2.0);
		int bottom = (int)(1 + pos.y + m_dCellHeight / 2.0);

		RectFloat cellRect = { (float)left, (float)top, (float)right, (float)bottom };

		switch (m_TerrainType[nd])
		{
		case obstacle:
			dc.DrawSprite(m_obstacleTexture, 0, 0xffffffff, (float)pos.x, (float)pos.y, CELL_SIZE, CELL_SIZE, dir);
			break;

		case water:
			dc.DrawSprite(m_waterTexture, 4, 0xffffffff, (float)pos.x, (float)pos.y, dir);
			break;

		case mud:
			dc.DrawSprite(m_mudTexture, 0, 0xffffffff, (float)pos.x, (float)pos.y, CELL_SIZE, CELL_SIZE, dir);
			break;

		default: break;
		}

		if (nd == m_iTargetCell)
			dc.DrawSprite(m_startPointTexture, 0, 0xffffffff, (float)pos.x, (float)pos.y, dir);

		if (nd == m_iSourceCell)
			dc.DrawSprite(m_finishPointTexture, 0, 0xffffffff, (float)pos.x, (float)pos.y, dir);
	}

	if (m_bShowGraph)
	{
		if (m_pGraph->NumNodes() != 0)
		{
			/*  if (DrawNodeIDs)
			{
				gdi->TextColor(200,200,200);
				gdi->TextAtPos((int)pN->Pos().x+5, (int)pN->Pos().y-5, ttos(pN->Index()));
			}*/

			//draw the nodes 
			NavGraph::ConstNodeIterator NodeItr(*m_pGraph);
			for (const NavGraph::NodeType* pN = NodeItr.begin(); !NodeItr.end(); pN = NodeItr.next())
			{
				NavGraph::ConstEdgeIterator EdgeItr(*m_pGraph, pN->Index());
				for (const NavGraph::EdgeType* pE = EdgeItr.begin(); !EdgeItr.end(); pE = EdgeItr.next())
				{
					Vector2D from = pN->Pos();
					Vector2D to = m_pGraph->GetNode(pE->To()).Pos();

					dc.DrawLine(m_lineTexture, 0xffffffff, from.x, from.y, to.x, to.y, 0);
				}
			}
		}
	}

	if (m_bShowTiles)
	{
		if (m_pGraph->NumNodes() != 0)
		{
			NavGraph::ConstNodeIterator NodeItr(*m_pGraph);
			for (const NavGraph::NodeType* pN = NodeItr.begin(); !NodeItr.end(); pN = NodeItr.next())
			{
				math::RectFloat dst = { (float)pN->Pos().x - 2, (float)pN->Pos().y - 2, (float)pN->Pos().x + 2, (float)pN->Pos().y + 2 };
				dc.DrawSprite(&dst, 0U, SpriteColor(255, 0, 0, 255), 0U); // tile's center
			}
		}
	}

	//draw any tree retrieved from the algorithms
	for (unsigned int e = 0; e < m_SubTree.size(); ++e)
	{
		if (m_SubTree[e])
		{
			Vector2D from = m_pGraph->GetNode(m_SubTree[e]->From()).Pos();
			Vector2D to = m_pGraph->GetNode(m_SubTree[e]->To()).Pos();

			dc.DrawLine(m_lineTexture, SpriteColor(255, 0, 0, 127), from.x, from.y, to.x, to.y, 0);
		}
	}

	//draw the path (if any)  
	if (m_Path.size() > 0)
	{
		std::list<int>::iterator it = m_Path.begin();
		std::list<int>::iterator nxt = it;

		for (++nxt; nxt != m_Path.end(); ++it, ++nxt)
		{
			Vector2D from = m_pGraph->GetNode(*it).Pos();
			Vector2D to = m_pGraph->GetNode(*nxt).Pos();

			dc.DrawLine(m_lineTexture, SpriteColor(0, 255, 0, 127), from.x, from.y, to.x, to.y, 0);
		}
	}
}
