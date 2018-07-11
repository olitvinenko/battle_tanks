#include "Raven_Map.h"
#include "Raven_ObjectEnumerations.h"
#include "misc/Cgdi.h"
#include "Graph/HandyGraphFunctions.h"
#include "Raven_Door.h"
#include "EntityManager.h"
#include "constants.h"
#include "Raven_Scriptor.h"

#include "triggers/Trigger_HealthGiver.h"
#include "triggers/Trigger_WeaponGiver.h"
#include "triggers/Trigger_OnButtonSendMsg.h"
#include "triggers/Trigger_SoundNotify.h"

#include "Raven_UserOptions.h"
#include "DrawingContext.h"
#include "TextureManager.h"


//uncomment to write object creation/deletion to debug console
#define  LOG_CREATIONAL_STUFF
#include "debug/DebugConsole.h"


Raven_Map::Raven_Map(int width, int heigt, TextureManager& texManager)
	: m_tm(texManager)
{
	FindTextures(texManager);

	//TODO:: scaling
	m_iSizeX = width;
	m_iSizeY = heigt;
}

//----------------------------- ctor ------------------------------------------
//-----------------------------------------------------------------------------
Raven_Map::Raven_Map(TextureManager& texManager)
					   :m_pNavGraph(NULL),
						_lineTexture(texManager.FindSprite("lightning")),
						_fontTexture(texManager.FindSprite("font_small")),
						_texBack(texManager.FindSprite("background")),
                       m_pSpacePartition(NULL),
                       m_iSizeY(0),
                       m_iSizeX(0),
                       m_dCellSpaceNeighborhoodRange(0),
                       m_tm(texManager)
{
	FindTextures(texManager);
}
//------------------------------ dtor -----------------------------------------
//-----------------------------------------------------------------------------
Raven_Map::~Raven_Map()
{
  Clear();
}

void Raven_Map::FindTextures(const TextureManager& tm)
{
	_lineTexture = tm.FindSprite("lightning");
	_fontTexture = tm.FindSprite("font_small");
	_texBack = tm.FindSprite("background");
	_spawnPointTexture = tm.FindSprite("item_mine");
}

//---------------------------- Clear ------------------------------------------
//
//  deletes all the current objects ready for a map load
//-----------------------------------------------------------------------------
void Raven_Map::Clear()
{
  //delete the triggers
  m_TriggerSystem.Clear();

  //delete the doors
  std::vector<Raven_Door*>::iterator curDoor = m_Doors.begin();
  for (curDoor; curDoor != m_Doors.end(); ++curDoor)
  {
    delete *curDoor;
  }

  m_Doors.clear();

  std::vector<Wall2D*>::iterator curWall = m_Walls.begin();
  for (curWall; curWall != m_Walls.end(); ++curWall)
  {
    delete *curWall;
  }

  m_Walls.clear();
  m_SpawnPoints.clear();
  
  //delete the navgraph
  delete m_pNavGraph;   

  //delete the partioning info
  delete m_pSpacePartition;
}


//----------------------------- AddWall ---------------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::AddWall(std::fstream& in)
{
  m_Walls.push_back(new Wall2D(in, m_tm));
}

Wall2D* Raven_Map::AddWall(Vector2D from, Vector2D to)
{
  Wall2D* w = new Wall2D(from, to, m_tm);

  m_Walls.push_back(w);

  return w;
}

//--------------------------- AddDoor -----------------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::AddDoor(std::fstream& in)
{
  Raven_Door* pDoor = new Raven_Door(this, in);

  m_Doors.push_back(pDoor);

  //register the entity 
  EntityMgr->RegisterEntity(pDoor);
}

//--------------------------- AddDoorTrigger ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::AddDoorTrigger(std::fstream& in)
{
  Trigger_OnButtonSendMsg<Raven_Bot>* tr = new Trigger_OnButtonSendMsg<Raven_Bot>(in);

  m_TriggerSystem.Register(tr);

  //register the entity 
  EntityMgr->RegisterEntity(tr);
  
}


//---------------------------- AddSpawnPoint ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::AddSpawnPoint(std::fstream& in)
{
  double x, y, dummy;

  in >> dummy >> x >> y >> dummy >> dummy;                   //dummy values are artifacts from the map editor

  m_SpawnPoints.push_back(Vector2D(x,y));
}


//----------------------- AddHealth__Giver ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::AddHealth_Giver(std::fstream& in)
{
  Trigger_HealthGiver* hg = new Trigger_HealthGiver(in);

  m_TriggerSystem.Register(hg);

  //let the corresponding navgraph node point to this object
  NavGraph::NodeType& node = m_pNavGraph->GetNode(hg->GraphNodeIndex());

  node.SetExtraInfo(hg);

  //register the entity 
  EntityMgr->RegisterEntity(hg);
}

//----------------------- AddWeapon__Giver ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::AddWeapon_Giver(int type_of_weapon, std::fstream& in)
{
  Trigger_WeaponGiver* wg = new Trigger_WeaponGiver(in);

  wg->SetEntityType(type_of_weapon);

  //add it to the appropriate vectors
  m_TriggerSystem.Register(wg);

  //let the corresponding navgraph node point to this object
  NavGraph::NodeType& node = m_pNavGraph->GetNode(wg->GraphNodeIndex());

  node.SetExtraInfo(wg);

  //register the entity 
  EntityMgr->RegisterEntity(wg);
}


//------------------------- LoadMap ------------------------------------
//
//  sets up the game environment from map file
//-----------------------------------------------------------------------------
bool Raven_Map::LoadMap(std::fstream& in)
{  
  if (!in)
  {
    ErrorBox("Bad Map Filename");
    return false;
  }

  Clear();

  GameEntityBase::ResetNextValidID();

  //first of all read and create the navgraph. This must be done before
  //the entities are read from the map file because many of the entities
  //will be linked to a graph node (the graph node will own a pointer
  //to an instance of the entity)
  m_pNavGraph = new NavGraph(false);
  
  m_pNavGraph->Load(in);

#ifdef LOG_CREATIONAL_STUFF
   // debug_con << "NavGraph for " << filename << " loaded okay" << "";
#endif

  //determine the average distance between graph nodes so that we can
  //partition them efficiently
  m_dCellSpaceNeighborhoodRange = CalculateAverageGraphEdgeLength(*m_pNavGraph) + 1;

#ifdef LOG_CREATIONAL_STUFF
    debug_con << "Average edge length is " << CalculateAverageGraphEdgeLength(*m_pNavGraph) << "";
#endif

#ifdef LOG_CREATIONAL_STUFF
    debug_con << "Neighborhood range set to " << m_dCellSpaceNeighborhoodRange << "";
#endif


  //load in the map size and adjust the client window accordingly
  in >> m_iSizeX >> m_iSizeY;

#ifdef LOG_CREATIONAL_STUFF
    debug_con << "Partitioning navgraph nodes..." << "";
#endif

  //partition the graph nodes
  PartitionNavGraph();


  //get the handle to the game window and resize the client area to accommodate
  //the map
  char* g_szApplicationName = "fsd";
  char* g_szWindowClassName = " sd";
  HWND hwnd = FindWindow(g_szWindowClassName, g_szApplicationName);
  const int ExtraHeightRqdToDisplayInfo = 50;
  ResizeWindow(hwnd, m_iSizeX, m_iSizeY+ExtraHeightRqdToDisplayInfo);

#ifdef LOG_CREATIONAL_STUFF
    debug_con << "Loading map..." << "";
#endif

 
  //now create the environment entities
  while (!in.eof())
  {   
    //get type of next map object
    int EntityType;
    
    in >> EntityType;

#ifdef LOG_CREATIONAL_STUFF
    debug_con << "Creating a " << GetNameOfType(EntityType) << "";
#endif

    //create the object
    switch(EntityType)
    {
    case type_wall:
 
        AddWall(in); break;

    case type_sliding_door:
 
        AddDoor(in); break;

    case type_door_trigger:
 
        AddDoorTrigger(in); break;

   case type_spawn_point:
     
       AddSpawnPoint(in); break;

   case type_health:
     
       AddHealth_Giver(in); break;

   case type_shotgun:
     
       AddWeapon_Giver(type_shotgun, in); break;

   case type_rail_gun:
     
       AddWeapon_Giver(type_rail_gun, in); break;

   case type_rocket_launcher:
     
       AddWeapon_Giver(type_rocket_launcher, in); break;

    default:
      
      throw std::runtime_error("<Map::Load>: Attempting to load undefined object");

      return false;
      
    }//end switch
  }

//#ifdef LOG_CREATIONAL_STUFF
//    debug_con << filename << " loaded okay" << "";
//#endif

   //calculate the cost lookup table
  m_PathCosts = CreateAllPairsCostsTable(*m_pNavGraph);

  return true;
}


bool Raven_Map::SaveMap(std::fstream& out) const
{
	//std::ofstream f(FileName.c_str());

	//TODO::

	//f.close();
	return false;
}



//------------- CalculateCostToTravelBetweenNodes -----------------------------
//
//  Uses the pre-calculated lookup table to determine the cost of traveling
//  from nd1 to nd2
//-----------------------------------------------------------------------------
double 
Raven_Map::CalculateCostToTravelBetweenNodes(int nd1, int nd2)const
{
  assert (nd1>=0 && nd1<m_pNavGraph->NumNodes() &&
          nd2>=0 && nd2<m_pNavGraph->NumNodes() &&
          "<Raven_Map::CostBetweenNodes>: invalid index");

  return m_PathCosts[nd1][nd2];
}




//-------------------------- PartitionEnvironment -----------------------------
//-----------------------------------------------------------------------------
void Raven_Map::PartitionNavGraph()
{
  if (m_pSpacePartition) delete m_pSpacePartition;

  m_pSpacePartition = new CellSpacePartition<NavGraph::NodeType*>(m_iSizeX,
                                                                  m_iSizeY,
                                                                  script->GetInt("NumCellsX"),
                                                                  script->GetInt("NumCellsY"),
                                                                  m_pNavGraph->NumNodes());

  //add the graph nodes to the space partition
  NavGraph::NodeIterator NodeItr(*m_pNavGraph);
  for (NavGraph::NodeType* pN=NodeItr.begin();!NodeItr.end();pN=NodeItr.next())
  {
    m_pSpacePartition->AddEntity(pN);
  }   
}

//---------------------------- AddSoundTrigger --------------------------------
//
//  given the bot that has made a sound, this method adds a SoundMade trigger
//-----------------------------------------------------------------------------
void Raven_Map::AddSoundTrigger(Raven_Bot* pSoundSource, double range)
{
  m_TriggerSystem.Register(new Trigger_SoundNotify(pSoundSource, range));
}

//----------------------- UpdateTriggerSystem ---------------------------------
//
//  givena container of entities in the world this method updates them against
//  all the triggers
//-----------------------------------------------------------------------------
void Raven_Map::UpdateTriggerSystem(std::list<Raven_Bot*>& bots)
{
  m_TriggerSystem.Update(bots);
}

//------------------------- GetRandomNodeLocation -----------------------------
//
//  returns the position of a graph node selected at random
//-----------------------------------------------------------------------------
Vector2D Raven_Map::GetRandomNodeLocation()const
{
  NavGraph::ConstNodeIterator NodeItr(*m_pNavGraph);
  int RandIndex = RandInt(0, m_pNavGraph->NumActiveNodes()-1);
  const NavGraph::NodeType* pN = NodeItr.begin();
  while (--RandIndex > 0)
  {
    pN = NodeItr.next();
  }

  return pN->Pos();
}


//--------------------------- Render ------------------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::Render(DrawingContext& dc, bool editor)
{
	//back
	dc.DrawBackground(_texBack, GetSizeX(), GetSizeY());

	//TODO:: move to smth like this: m_pNavGraph->Render(dc, editor);
   //render the navgraph
	if (editor && m_pNavGraph->NumNodes() != 0)
	{
		SpriteColor c(255.0f, 255.0f, 255.0f, 255.0f);

		NavGraph::ConstNodeIterator NodeItr(*m_pNavGraph);
		for (const NavGraph::NodeType* pN = NodeItr.begin();
			!NodeItr.end();
			pN = NodeItr.next())
		{
			if (UserOptions->m_bShowGraph)
			{
				NavGraph::ConstEdgeIterator EdgeItr(*m_pNavGraph, pN->Index());
				for (const NavGraph::EdgeType* pE = EdgeItr.begin();
					!EdgeItr.end();
					pE = EdgeItr.next())
				{
					Vector2D from = pN->Pos();
					Vector2D to = m_pNavGraph->GetNode(pE->To()).Pos();

					dc.DrawLine(_lineTexture, c, from.x, from.y, to.x, to.y, 0);
				}
			}

			if (UserOptions->m_bShowNodeIndices)
			{
				math::RectFloat dst = { (float)pN->Pos().x - 2, (float)pN->Pos().y - 2, (float)pN->Pos().x + 2, (float)pN->Pos().y + 2 };
				dc.DrawSprite(&dst, 0U, 0xffffffff, 0U);
			}
		}

		if (UserOptions->m_bShowNodeIndices)
		{
			for (const NavGraph::NodeType* pN = NodeItr.begin();
				!NodeItr.end();
				pN = NodeItr.next())
			{
				dc.DrawBitmapText((int)pN->Pos().x + 5, (int)pN->Pos().y - 5, _fontTexture, c, std::to_string(pN->Index()), alignTextLT);
			}
		}
	}

  //render any doors
  std::vector<Raven_Door*>::iterator curDoor = m_Doors.begin();
  for (curDoor; curDoor != m_Doors.end(); ++curDoor)
  {
    (*curDoor)->Render(dc);
  }

  //TODO:: implement
  //render all the triggers
  m_TriggerSystem.Render(dc);

  //render all the walls
  std::vector<Wall2D*>::const_iterator curWall = m_Walls.begin();
  for (curWall; curWall != m_Walls.end(); ++curWall)
  {
    (*curWall)->Render(dc);
  }

  std::vector<Vector2D>::const_iterator curSp = m_SpawnPoints.begin();
  for (curSp; curSp != m_SpawnPoints.end(); ++curSp)
  {
	  math::RectFloat dst = { (float)curSp->x - 7, (float)curSp->y - 7, (float)curSp->x + 7, (float)curSp->y + 7 };
	  dc.DrawSprite(&dst, 0U, 0xffffffff, 0U);

	//  dc.DrawSprite(_spawnPointTexture, 0, 0xffffffff, (float)curSp->x, (float)curSp->y, (float)7, (float)7, math::Vector2());
  }
}
