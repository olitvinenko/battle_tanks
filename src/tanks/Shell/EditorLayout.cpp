#include "DefaultCamera.h"
#include "EditorLayout.h"
#include "Config.h"

#include "WorldCfg.h"
#include "WorldView.h"
#include "GuiManager.h"
#include "Text.h"
#include "Combo.h"
#include "ConsoleBuffer.h"
#include "List.h"
#include "DataSourceAdapters.h"
#include "Keys.h"

#include <sstream>
#include <algorithm>
#include <functional>
#include "Z.h"
#include "Rendering/TextureManager.h"

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}


///////////////////////////////////////////////////////////////////////////////

//static bool PtInRigidBody(const GC_RigidBodyStatic &rbs, vec2d delta)
//{
//	if (fabs(Vec2dDot(delta, rbs.GetDirection())) > rbs.GetHalfLength())
//		return false;
//	if (fabs(Vec2dCross(delta, rbs.GetDirection())) > rbs.GetHalfWidth())
//		return false;
//	return true;
//}
//
//static bool PtInPickup(const GC_Pickup &pickup, vec2d delta)
//{
//	float r = pickup.GetRadius();
//	if (Vec2dDot(delta, delta) > r*r)
//		return false;
//	return true;
//}
//
//static bool PtInDefaultRadius(vec2d delta)
//{
//	float r = 8;
//	if (Vec2dDot(delta, delta) > r*r)
//		return false;
//	return true;
//}
//
//static bool PtInActor(const GC_Actor &actor, vec2d pt)
//{
//	vec2d delta = pt - actor.GetPos();
//	if (PtInDefaultRadius(delta))
//		return true;
//	if (auto rbs = dynamic_cast<const GC_RigidBodyStatic*>(&actor))
//		return PtInRigidBody(*rbs, delta);
//	if (auto pickup = dynamic_cast<const GC_Pickup*>(&actor))
//		return PtInPickup(*pickup, delta);
//	return false;
//}
//
//static GC_Actor* PickEdObject(const RenderScheme &rs, World &world, const vec2d &pt, int layer)
//{
//    GC_Actor* zLayers[Z_COUNT];
//    memset(zLayers, 0, sizeof(zLayers));
//
//    std::vector<ObjectList*> receive;
//    world.grid_actors.OverlapPoint(receive, pt / LOCATION_SIZE);
//    for( auto rit = receive.begin(); rit != receive.end(); ++rit )
//    {
//        ObjectList *ls = *rit;
//        for( auto it = ls->begin(); it != ls->end(); it = ls->next(it) )
//        {
//            auto *object = static_cast<GC_Actor*>(ls->at(it));
//			if (PtInActor(*object, pt))
//            {
//				enumZOrder maxZ = Z_NONE;
//				if( const ObjectViewsSelector::ViewCollection *views = rs.GetViews(*object, true, false) )
//				{
//					for (auto &view: *views)
//					{
//						maxZ = std::max(maxZ, view.zfunc->GetZ(world, *object));
//					}
//				}
//
//				if( Z_NONE != maxZ )
//				{
//					for( unsigned int i = 0; i < RTTypes::Inst().GetTypeCount(); ++i )
//					{
//						if( object->GetType() == RTTypes::Inst().GetTypeByIndex(i)
//							&& (-1 == layer || RTTypes::Inst().GetTypeInfoByIndex(i).layer == layer) )
//						{
//							zLayers[maxZ] = object;
//						}
//					}
//				}
//            }
//        }
//    }
//
//    for( int z = Z_COUNT; z--; )
//    {
//        if (zLayers[z])
//            return zLayers[z];
//    }
//
//    return nullptr;
//}


EditorLayout::EditorLayout(UIWindow *parent, Pathfinder& pathfinder , WorldView &worldView, const DefaultCamera &defaultCamera, lua_State *globL, UI::ConsoleBuffer &logger)
  : UIWindow(parent)
  , _logger(logger)
  , _defaultCamera(defaultCamera)
  , _fontSmall(GetManager().GetTextureManager().FindSprite("font_small"))
  , _texSelection(GetManager().GetTextureManager().FindSprite("ui/selection"))
 // , _selectedObject(nullptr)
  , _isObjectNew(false)
  , _click(true)
  , _mbutton(0)
  , _pathfinder(pathfinder)
  , _worldView(worldView)
  , _globL(globL)
{
	SetTexture(nullptr, false);

	_drawGraph = UI::CheckBox::Create(this, 10, 50, "Show graph");
	_drawGraph->eventClick = [=]() { _pathfinder.ToggleShowGraph(_drawGraph->GetCheck()); };
	_drawGraph->SetCheck(false);

	_drawTiles = UI::CheckBox::Create(this, 10, 50, "Show tiles");
	_drawTiles->eventClick = [=]() { _pathfinder.ToggleShowTiles(_drawTiles->GetCheck()); };
	_drawTiles->SetCheck(false);

	_help = UI::Text::Create(this, 10, 10, "Help"/*_lang.f1_help_editor.Get()*/, alignTextLT);
	_help->SetVisible(false);

	//_propList = new PropertyList(this, 5, 5, 512, 256, _world, _conf, _logger);
	//_propList->SetVisible(false);

	//_serviceList = new ServiceEditor(this, 5, 300, 512, 256, _world, _conf, _lang);
	//_serviceList->SetVisible(_conf.ed_showservices.Get());

	_layerDisp = UI::Text::Create(this, 0, 0, "", alignTextRT);

	_typeList = DefaultComboBox::Create(this);
	_typeList->Resize(256);
	for (unsigned int i = 0; i < Pathfinder::brush_type::count; ++i )
	{
		switch (static_cast<Pathfinder::brush_type>(i))
		{
		case Pathfinder::normal: _typeList->GetData()->AddItem("none"); break;
		case Pathfinder::obstacle: _typeList->GetData()->AddItem("obstacle"); break;
		case Pathfinder::water:_typeList->GetData()->AddItem("water"); break;
		case Pathfinder::mud:_typeList->GetData()->AddItem("mud"); break;
		case Pathfinder::source:_typeList->GetData()->AddItem("source"); break;
		case Pathfinder::target:_typeList->GetData()->AddItem("target"); break;

		default: break;
		}
		//if( RTTypes::Inst().GetTypeInfoByIndex(i).service ) continue;
		//const char *desc0 = RTTypes::Inst().GetTypeInfoByIndex(i).desc;
		//_typeList->GetData()->AddItem(_lang->GetStr(desc0).Get(), RTTypes::Inst().GetTypeByIndex(i));
	}
	//_typeList->GetData()->Sort();
	UI::List *ls = _typeList->GetList();
	ls->SetTabPos(1, 128);
	ls->AlignHeightToContent();
	//_typeList->eventChangeCurSel = std::bind(&EditorLayout::OnChangeObjectType, this, std::placeholders::_1);
	_typeList->eventChangeCurSel = [=](int index)
	{
		_pathfinder.ChangeBrush((Pathfinder::brush_type) index);
	};
	_typeList->SetCurSel(/*std::min(_typeList->GetData()->GetItemCount() - 1, std::max(0, 1/*_conf.ed_object.GetInt()))*/0);

	_algosList = DefaultComboBox::Create(this);
	_algosList->Resize(256);
	for (unsigned int i = 0; i < Pathfinder::algorithm_type::max; ++i)
	{
		switch (static_cast<Pathfinder::algorithm_type>(i))
		{
		case Pathfinder::algorithm_type::non: _algosList->GetData()->AddItem("none"); break;
		case Pathfinder::algorithm_type::search_bfs: _algosList->GetData()->AddItem("BFS"); break;
		case Pathfinder::algorithm_type::search_dfs: _algosList->GetData()->AddItem("DFS"); break;
		case Pathfinder::algorithm_type::search_dijkstra: _algosList->GetData()->AddItem("Dijkstra"); break;
		case Pathfinder::algorithm_type::search_astar: _algosList->GetData()->AddItem("AStar"); break;
		}
	}
	ls = _algosList->GetList();
	ls->SetTabPos(1, 128);
	ls->AlignHeightToContent();
	_algosList->eventChangeCurSel = [=](int index)
	{
		switch (static_cast<Pathfinder::algorithm_type>(index))
		{
		case Pathfinder::algorithm_type::search_dfs: _pathfinder.CreatePathDFS(); break;
		case Pathfinder::algorithm_type::search_bfs: _pathfinder.CreatePathBFS(); break;
		case Pathfinder::algorithm_type::search_dijkstra: _pathfinder.CreatePathDijkstra(); break;
		case Pathfinder::algorithm_type::search_astar:_pathfinder.CreatePathAStar(); break;
		}
	};
	_algosList->SetCurSel(0);

	//assert(!_conf.ed_uselayers.eventChange);
	//_conf.ed_uselayers.eventChange = std::bind(&EditorLayout::OnChangeUseLayers, this);
	OnChangeUseLayers();
}

EditorLayout::~EditorLayout()
{
	//_conf.ed_uselayers.eventChange = nullptr;
}

void EditorLayout::OnKillSelected(/*World &world,/* GC_Object *sender,*/ void *param)
{
	//Select(sender, false);
}

void EditorLayout::OnMoveSelected(/*World &world,/* GC_Object *sender,*/ void *param)
{
	//assert(_selectedObject == sender);
}

//void EditorLayout::Select(GC_Object *object, bool bSelect)
//{
	//assert(object);

	//if( bSelect )
	//{
	//	if( _selectedObject != object )
	//	{
	//		if( _selectedObject )
	//		{
	//			Select(_selectedObject, false);
	//		}

	//		_selectedObject = object;
	//		_propList->ConnectTo(_selectedObject->GetProperties(_world));
	//		if( _conf.ed_showproperties.Get() )
	//		{
	//			_propList->SetVisible(true);
	//		}
	//	}
	//}
	//else
	//{
	//	assert(object == _selectedObject);
	//	_selectedObject = nullptr;
	//	_isObjectNew = false;

	//	_propList->ConnectTo(nullptr);
	//	_propList->SetVisible(false);
	//}

	//if( eventOnChangeSelection )
	//	eventOnChangeSelection(_selectedObject);
//}

void EditorLayout::SelectNone()
{
	//if( _selectedObject )
	//{
	//	Select(_selectedObject, false);
	//}
}

bool EditorLayout::OnMouseWheel(float x, float y, float z)
{
	//if( z > 0 )
	//{
	//	_typeList->SetCurSel( std::max(0, _typeList->GetCurSel() - 1) );
	//}
	//if( z < 0 )
	//{
	//	_typeList->SetCurSel( std::min(_typeList->GetData()->GetItemCount()-1, _typeList->GetCurSel() + 1) );
	//}
	return true;
}

bool EditorLayout::OnPointerMove(float x, float y, UI::PointerType pointerType, unsigned int pointerID)
{
	if( _mbutton )
	{
		OnPointerDown(x, y, _mbutton, pointerType, pointerID);
	}
	return true;
}

bool EditorLayout::OnPointerUp(float x, float y, int button, UI::PointerType pointerType, unsigned int pointerID)
{
	if( _mbutton == button )
	{
		_click = true;
		_mbutton = 0;
		GetManager().SetCapture(pointerID, nullptr);
	}
	return true;
}

bool EditorLayout::OnPointerDown(float x, float y, int button, UI::PointerType pointerType, unsigned int pointerID)
{
	if (_mbutton == 0)
	{
		GetManager().SetCapture(pointerID, this);
		_mbutton = button;
	}


	if (_mbutton != button)
	{
		return true;
	}

    Vector2 mouse(x / _defaultCamera.GetZoom() + _defaultCamera.GetPos().x,
                y / _defaultCamera.GetZoom() + _defaultCamera.GetPos().y);

	_pathfinder.PaintTerrain(mouse);

 //   ObjectType type = static_cast<ObjectType>(
 //       _typeList->GetData()->GetItemData(_conf.ed_object.GetInt()) );

 //   float align = RTTypes::Inst().GetTypeInfo(type).align;
 //   float offset = RTTypes::Inst().GetTypeInfo(type).offset;

 //   vec2d pt;
 //   pt.x = std::min(_world._sx - align, std::max(align - offset, mouse.x));
 //   pt.y = std::min(_world._sy - align, std::max(align - offset, mouse.y));
 //   pt.x = pt.x - fmod(pt.x + align * 0.5f - offset, align) - align * 0.5f;
 //   pt.y = pt.y - fmod(pt.y + align * 0.5f - offset, align) - align * 0.5f;

 //   int layer = -1;
 //   if( _conf.ed_uselayers.Get() )
 //   {
 //       layer = RTTypes::Inst().GetTypeInfo(_typeList->GetData()->GetItemData(_typeList->GetCurSel())).layer;
 //   }

 //   if( GC_Object *object = PickEdObject(_worldView.GetRenderScheme(), _world, mouse, layer) )
 //   {
 //       if( 1 == button )
 //       {
 //           if( _click && _selectedObject == object )
 //           {
 //               auto &selTypeInfo = RTTypes::Inst().GetTypeInfo(object->GetType());

 //               lua_getglobal(_globL, "editor_actions");
 //               if( lua_isnil(_globL, -1) )
 //               {
 //                   _logger.WriteLine(1, "There was no editor module loaded");
 //               }
 //               else
 //               {
 //                   lua_getfield(_globL, -1, selTypeInfo.name);
 //                   if (lua_isnil(_globL, -1))
 //                   {
 //                       // no action available for this object
 //                       lua_pop(_globL, 1);
 //                   }
 //                   else
 //                   {
 //                       luaT_pushobject(_globL, object);
 //                       if( lua_pcall(_globL, 1, 0, 0) )
 //                       {
 //                           _logger.WriteLine(1, lua_tostring(_globL, -1));
 //                           lua_pop(_globL, 1); // pop error message
 //                       }
 //                   }
 //               }
 //               lua_pop(_globL, 1); // pop editor_actions

 //               _propList->DoExchange(false);
 //               if( _isObjectNew )
 //                   SaveToConfig(_conf, *object->GetProperties(_world));
 //           }
 //           else
 //           {
 //               Select(object, true);
 //           }
 //       }

 //       if( 2 == button )
 //       {
 //           if( _selectedObject == object )
 //           {
 //               Select(object, false);
 //           }
 //           object->Kill(_world);
 //       }
 //   }
 //   else
 //   {
 //       if( 1 == button )
 //       {
 //           // create object
 //           GC_Actor &newobj = RTTypes::Inst().CreateActor(_world, type, pt.x, pt.y);
	//		std::shared_ptr<PropertySet> properties = newobj.GetProperties(_world);

 //           // set default properties if Ctrl key is not pressed
 //           if( GetManager().GetInput().IsKeyPressed(UI::Key::LeftCtrl) ||
 //               GetManager().GetInput().IsKeyPressed(UI::Key::RightCtrl) )
 //           {
 //               LoadFromConfig(_conf, *properties);
 //               properties->Exchange(_world, true);
 //           }

	//		Select(&newobj, true);
 //           _isObjectNew = true;
 //       }
 //   }

	//_click = false;
	return true;
}

bool EditorLayout::OnFocus(bool focus)
{
	return true;
}

bool EditorLayout::OnKeyPressed(Key key)
{
	//switch(key)
	//{
	//case UI::Key::Enter:
	//	if( _selectedObject )
	//	{
	//		_propList->SetVisible(true);
	//		_conf.ed_showproperties.Set(true);
	//	}
	//	break;
	//case UI::Key::S:
	//	_serviceList->SetVisible(!_serviceList->GetVisible());
	//	_conf.ed_showservices.Set(_serviceList->GetVisible());
	//	break;
	//case UI::Key::Delete:
	//	if( _selectedObject )
	//	{
	//		GC_Object *o = _selectedObject;
	//		Select(_selectedObject, false);
	//		o->Kill(_world);
	//	}
	//	break;
	//case UI::Key::F1:
	//	_help->SetVisible(!_help->GetVisible());
	//	break;
	//case UI::Key::F8:
	////	dlg = new MapSettingsDlg(this, _world);
	////	SetDrawBackground(true);
	////	dlg->eventClose = std::bind(&Desktop::OnCloseChild, this, std::placeholders::_1);
	////	_nModalPopups++;
	//	break;

	//case UI::Key::F9:
	//	_conf.ed_uselayers.Set(!_conf.ed_uselayers.Get());
	//	break;
	//case UI::Key::G:
	//	_conf.ed_drawgrid.Set(!_conf.ed_drawgrid.Get());
	//	break;
	//case UI::Key::Escape:
	//	if( _selectedObject )
	//	{
	//		Select(_selectedObject, false);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//	break;
	//default:
	//	return false;
	//}
	return true;
}

void EditorLayout::OnSize(float width, float height)
{
	float w = width - _typeList->GetWidth() - 5;

	_typeList->Move(w , 5);
	_layerDisp->Move(w, 6);

	_algosList->Move(w, _typeList->GetHeight() + 2);

	_drawGraph->Move(w, _typeList->GetHeight() + 20);
	_drawTiles->Move(w, _typeList->GetHeight() + 40);
}

void EditorLayout::OnVisibleChange(bool visible, bool inherited)
{
	if( !visible )
	{
		SelectNone();
	}
}

void EditorLayout::OnChangeObjectType(int index)
{
	//_conf.ed_object.SetInt(index);

	//std::ostringstream buf;
	//buf << _lang.layer.Get() << RTTypes::Inst().GetTypeInfo(_typeList->GetData()->GetItemData(index)).layer << ": ";
	//_layerDisp->SetText(buf.str());
}

void EditorLayout::OnChangeUseLayers()
{
//	_layerDisp->SetVisible(_conf.ed_uselayers.Get());
}

//static FRECT GetSelectionRect(const GC_Actor &actor)
//{
//	float halfSize = 8;
//	if (auto *pickup = dynamic_cast<const GC_Pickup*>(&actor))
//		halfSize = pickup->GetRadius();
//	else if (auto *rbs = dynamic_cast<const GC_RigidBodyStatic*>(&actor))
//		halfSize = rbs->GetRadius();
//	FRECT result = {
//		actor.GetPos().x - halfSize,
//		actor.GetPos().y - halfSize,
//		actor.GetPos().x + halfSize,
//		actor.GetPos().y + halfSize
//	};
//	return result;
//}

void EditorLayout::Draw(DrawingContext &dc) const
{
	UIWindow::Draw(dc);

	math::RectInt viewport(0, 0, (int) GetWidth(), (int) GetHeight());
	Vector2 eye(_defaultCamera.GetPos().x + GetWidth() / 2, _defaultCamera.GetPos().y + GetHeight() / 2);
	float zoom = _defaultCamera.GetZoom();
	_worldView.Render(dc, viewport, eye, zoom, true, false, _pathfinder);

	dc.SetMode(INTERFACE);

	//if( auto *s = dynamic_cast<const GC_Actor *>(_selectedObject) )
	//{
	//	FRECT rt = GetSelectionRect(*s);

	//	FRECT sel = {
	//		(rt.left - _defaultCamera.GetPos().x) * _defaultCamera.GetZoom(),
	//		(rt.top - _defaultCamera.GetPos().y) * _defaultCamera.GetZoom(),
	//		(rt.left - _defaultCamera.GetPos().x) * _defaultCamera.GetZoom() + WIDTH(rt) * _defaultCamera.GetZoom(),
	//		(rt.top - _defaultCamera.GetPos().y) * _defaultCamera.GetZoom() + HEIGHT(rt) * _defaultCamera.GetZoom()
	//	};
	//	dc.DrawSprite(&sel, _texSelection, 0xffffffff, 0);
	//	dc.DrawBorder(sel, _texSelection, 0xffffffff, 0);
	//}
	Vector2 mouse = GetManager().GetInput().GetMousePosition() / _defaultCamera.GetZoom() + _defaultCamera.GetPos();

	std::stringstream buf;
	buf << "x=" << floor(mouse.x + 0.5f) << "; y=" << floor(mouse.y + 0.5f) << "; zoom= " << _defaultCamera.GetZoom() << "; cameraPos= " << _defaultCamera.GetPos();
	dc.DrawBitmapText(floor(GetWidth()/2+0.5f), 1, _fontSmall, 0xffffffff, buf.str(), alignTextCT);
}
