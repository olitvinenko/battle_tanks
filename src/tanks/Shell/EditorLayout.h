#pragma once
#include "UIWindow.h"
#include <functional>
#include "Raven_Game.h"
#include "Button.h"

class DefaultCamera;
class WorldView;
struct lua_State;

namespace UI
{
	template <class, class> class ListAdapter;
	class ListDataSourceDefault;
	class ComboBox;
	class ConsoleBuffer;
	class CheckBox;
	class Text;
}

class EditorLayout : public UI::UIWindow
{
	typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::ComboBox> DefaultComboBox;

	UI::ConsoleBuffer &_logger;
	const DefaultCamera &_defaultCamera;
	//PropertyList *_propList;
	//ServiceEditor    *_serviceList;
	UI::Text         *_layerDisp;
	DefaultComboBox  *_typeList;
	UI::Text         *_help;
	size_t        _fontSmall;

	UI::CheckBox * _drawGraph;
	UI::CheckBox * _drawIndices;

	size_t       _texSelection;

	//GC_Object *_selectedObject;
	bool _isObjectNew;
	bool _click;
	int  _mbutton;
	WorldView &_worldView;
	lua_State *_globL;
	Raven_Game & _game;


	void OnKillSelected(/*World &world, /*GC_Object *sender,*/ void *param);
	void OnMoveSelected(/*World &world, /*GC_Object *sender,*/ void *param);

public:
	EditorLayout(UI::UIWindow *parent, Raven_Game& game, WorldView &worldView, const DefaultCamera &defaultCamera, lua_State *globL, UI::ConsoleBuffer &logger);
	virtual ~EditorLayout();

	//void Select(GC_Object *object, bool bSelect);
	void SelectNone();

	//std::function<void(GC_Object*)> eventOnChangeSelection;

protected:
	void Draw(DrawingContext &dc) const override;

	bool OnMouseWheel(float x, float y, float z) override;
	bool OnPointerDown(float x, float y, int button, UI::PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerUp(float x, float y, int button, UI::PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerMove(float x, float y, UI::PointerType pointerType, unsigned int pointerID) override;
	bool OnFocus(bool focus) override;
	bool OnKeyPressed(UI::Key key) override;
	void OnSize(float width, float height) override;
	void OnVisibleChange(bool visible, bool inherited) override;

	void OnChangeObjectType(int index);
	void OnChangeUseLayers();
};
