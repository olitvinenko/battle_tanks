#pragma once
#include "gc/WorldEvents.h"
#include "ui/Dialog.h"
#include "ui/ListBase.h"
#include <string>

class LangCache;
namespace UI
{
	template <class, class> class ListAdapter;
	class ComboBox;
	class Button;
	class List;
	class Text;
}

class ServiceListDataSource
	: public UI::ListDataSource
	, public ObjectListener<World>
{
public:
	// UI::ListDataSource
	void AddListener(UI::ListDataSourceListener *listener) override;
	void RemoveListener(UI::ListDataSourceListener *listener) override;
	int GetItemCount() const override;
	int GetSubItemCount(int index) const override;
	size_t GetItemData(int index) const override;
	const std::string& GetItemText(int index, int sub) const override;
	int FindItem(const std::string &text) const override;

	// ObjectListener<World>
	void OnGameStarted() override {}
	void OnGameFinished() override {}
	void OnNewObject(GC_Object &obj) override;
	void OnKill(GC_Object &obj) override;

public:
	ServiceListDataSource(World &world, LangCache &lang);
	~ServiceListDataSource();

private:
	mutable std::string _nameCache;
	UI::ListDataSourceListener *_listener;
	World &_world;
	LangCache &_lang;
};
