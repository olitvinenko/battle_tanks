#include "ServiceEditor.h"
#include "Configuration.h"
#include "gc/Player.h"
#include "gc/TypeSystem.h"
#include "gc/World.h"
#include "loc/Language.h"
#include "ui/Button.h"
#include "ui/DataSourceAdapters.h"
#include "ui/Combo.h"
#include "ui/List.h"
#include "ui/Text.h"
#include "ui/Keys.h"

#include "Editor.h" // WTF! cross-reference

ServiceListDataSource::ServiceListDataSource(World &world, LangCache &lang)
	: _listener(nullptr)
	, _world(world)
	, _lang(lang)
{
	_world.eWorld.AddListener(*this);
}

ServiceListDataSource::~ServiceListDataSource()
{
	_world.eWorld.RemoveListener(*this);
}

void ServiceListDataSource::AddListener(UI::ListDataSourceListener *listener)
{
	_listener = listener;
}

void ServiceListDataSource::RemoveListener(UI::ListDataSourceListener *listener)
{
	assert(_listener && _listener == listener);
	_listener = nullptr;
}

int ServiceListDataSource::GetItemCount() const
{
	return _world.GetList(LIST_services).size();
}

int ServiceListDataSource::GetSubItemCount(int index) const
{
	return 2;
}

size_t ServiceListDataSource::GetItemData(int index) const
{
	auto it = _world.GetList(LIST_services).begin();
	for (int i = 0; i < index; ++i)
	{
		it = _world.GetList(LIST_services).next(it);
		assert(_world.GetList(LIST_services).end() != it);
	}
	return (size_t)_world.GetList(LIST_services).at(it);
}

const std::string& ServiceListDataSource::GetItemText(int index, int sub) const
{
	GC_Object *s = (GC_Object *)GetItemData(index);
	const char *name;
	switch (sub)
	{
	case 0:
		return _lang->GetStr(RTTypes::Inst().GetTypeInfo(s->GetType()).desc).Get();
	case 1:
		name = s->GetName(_world);
		_nameCache = name ? name : "";
		return _nameCache;
	}
	assert(false);
	_nameCache = "";
	return _nameCache;
}

int ServiceListDataSource::FindItem(const std::string &text) const
{
	return -1;
}

void ServiceListDataSource::OnNewObject(GC_Object &obj)
{
	if (obj.GetType() == GC_Player::GetTypeStatic())
		_listener->OnAddItem();
}

void ServiceListDataSource::OnKill(GC_Object &obj)
{
	if (obj.GetType() == GC_Player::GetTypeStatic())
	{
		ObjectList &list = _world.GetList(LIST_services);
		int found = -1;
		int idx = 0;
		for (auto it = list.begin(); it != list.end(); it = list.next(it))
		{
			if (list.at(it) == &obj)
			{
				found = idx;
				break;
			}
			++idx;
		}
		assert(-1 != found);

		_listener->OnDeleteItem(found);
	}
}

///////////////////////////////////////////////////////////////////////////////
