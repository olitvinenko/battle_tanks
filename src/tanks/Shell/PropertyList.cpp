#include "PropertyList.h"
#include "ConfigBinding.h"
#include "Configuration.h"
#include <Object.h>
#include <TypeSystem.h>
#include <WorldCfg.h>
#include <Language.h>
#include <Button.h>
#include <Combo.h>
#include <ConsoleBuffer.h>
#include <DataSource.h>
#include <DataSourceAdapters.h>
#include <Edit.h>
#include <GuiManager.h>
#include <Keys.h>
#include <LayoutContext.h>
#include <List.h>
#include <ListBase.h>
#include <ScrollView.h>
#include <StackLayout.h>
#include <Text.h>
#include <TextureManager.h>
#include <algorithm>

PropertyList::PropertyList(UI::LayoutManager &manager, TextureManager &texman, World &world, ShellConfig &conf, UI::ConsoleBuffer &logger, LangCache &lang)
	: Dialog(manager, texman)
	, _deleteButton(std::make_shared<UI::Button>(manager, texman))
	, _scrollView(std::make_shared<UI::ScrollView>(manager))
	, _psheet(std::make_shared<UI::StackLayout>(manager))
	, _world(world)
	, _conf(conf)
	, _logger(logger)
{
	SetTexture(texman, "ui/list", false);

	_deleteButton->SetText(ConfBind(lang.ed_delete));
	_deleteButton->eventClick = [this]
	{
		if( _ps && _ps->GetObject() )
			_ps->GetObject()->Kill(_world);
	};
	AddFront(_deleteButton);

	_scrollView->SetContent(_psheet);
	AddFront(_scrollView);

	_psheet->SetSpacing(10);
}

void PropertyList::DoExchange(bool applyToObject, TextureManager &texman)
{
	int focus = -1;

	if( applyToObject )
	{
		assert(_ps);
		for( int i = 0; i < _ps->GetCount(); ++i )
		{
			if (_psheet->GetFocus() == _psheet->GetChildren()[i])
			{
				focus = i;
			}

			ObjectProperty *prop = _ps->GetProperty(i);
			auto &ctrl = _ctrls[i];

			switch( prop->GetType() )
			{
			case ObjectProperty::TYPE_INTEGER:
				assert( dynamic_cast<UI::Edit*>(ctrl.get()) );
				int n;
				n = static_cast<UI::Edit*>(ctrl.get())->GetInt();
				if( n < prop->GetIntMin() || n > prop->GetIntMax() )
				{
					_logger.Printf(1, "WARNING: value %s out of range [%d, %d]",
						prop->GetName().c_str(), prop->GetIntMin(), prop->GetIntMax());
					n = std::max(prop->GetIntMin(), std::min(prop->GetIntMax(), n));
				}
				prop->SetIntValue(n);
				break;
			case ObjectProperty::TYPE_FLOAT:
				assert( dynamic_cast<UI::Edit*>(ctrl.get()) );
				float f;
				f = static_cast<UI::Edit*>(ctrl.get())->GetFloat();
				if( f < prop->GetFloatMin() || f > prop->GetFloatMax() )
				{
					_logger.Printf(1, "WARNING: value %s out of range [%g, %g]",
						prop->GetName().c_str(), prop->GetFloatMin(), prop->GetFloatMax());
					f = std::max(prop->GetFloatMin(), std::min(prop->GetFloatMax(), f));
				}
				prop->SetFloatValue(f);
				break;
			case ObjectProperty::TYPE_STRING:
				assert( dynamic_cast<UI::Edit*>(ctrl.get()) );
				prop->SetStringValue(static_cast<UI::Edit*>(ctrl.get())->GetText());
				break;
			case ObjectProperty::TYPE_MULTISTRING:
				assert( dynamic_cast<UI::ComboBox*>(ctrl.get()) );
				int index;
				index = static_cast<UI::ComboBox*>(ctrl.get())->GetCurSel();
				prop->SetCurrentIndex(index);
				break;
			case ObjectProperty::TYPE_SKIN:
			case ObjectProperty::TYPE_TEXTURE:
				assert( dynamic_cast<UI::ComboBox*>(ctrl.get()) );
				index = static_cast<UI::ComboBox*>(ctrl.get())->GetCurSel();
				prop->SetStringValue(static_cast<UI::ComboBox*>(ctrl.get())->GetData()->GetItemText(index, 0));
				break;
			default:
				assert(false);
			}
		}
		_ps->Exchange(_world, true);
	}


	// clear old controls
	_psheet->UnlinkAllChildren();
	_ctrls.clear();

	// create new controls
	if( _ps )
	{
		_ps->Exchange(_world, false);
		for( int i = 0; i < _ps->GetCount(); ++i )
		{
			ObjectProperty *prop = _ps->GetProperty(i);

			auto group = std::make_shared<UI::StackLayout>(GetManager());
			group->SetSpacing(2);
			_psheet->AddFront(group);

			auto label = std::make_shared<UI::Text>(GetManager(), texman);
			label->SetText(std::make_shared<UI::StaticText>(prop->GetName()));
			group->AddFront(label);

			std::shared_ptr<Window> ctrl;

			std::ostringstream labelTextBuffer;

			switch( prop->GetType() )
			{
			case ObjectProperty::TYPE_INTEGER:
				ctrl = std::make_shared<UI::Edit>(GetManager(), texman);
				std::static_pointer_cast<UI::Edit>(ctrl)->SetInt(prop->GetIntValue());
				labelTextBuffer << "(" << prop->GetIntMin() << " - " << prop->GetIntMax() << ")";
				break;
			case ObjectProperty::TYPE_FLOAT:
				ctrl = std::make_shared<UI::Edit>(GetManager(), texman);
				std::static_pointer_cast<UI::Edit>(ctrl)->SetFloat(prop->GetFloatValue());
				labelTextBuffer << "(" << prop->GetFloatMin() << " - " << prop->GetFloatMax() << ")";
				break;
			case ObjectProperty::TYPE_STRING:
				ctrl = std::make_shared<UI::Edit>(GetManager(), texman);
				std::static_pointer_cast<UI::Edit>(ctrl)->SetText(texman, prop->GetStringValue());
				labelTextBuffer << "(string)";
				break;
			case ObjectProperty::TYPE_MULTISTRING:
			case ObjectProperty::TYPE_SKIN:
			case ObjectProperty::TYPE_TEXTURE:
				typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::ComboBox> DefaultComboBox;
				ctrl = std::make_shared<DefaultComboBox>(GetManager(), texman);
				if (prop->GetType() == ObjectProperty::TYPE_MULTISTRING)
				{
					for( size_t index = 0; index < prop->GetListSize(); ++index )
					{
						static_cast<DefaultComboBox *>(ctrl.get())->GetData()->AddItem(prop->GetListValue(index));
					}
					static_cast<DefaultComboBox*>(ctrl.get())->SetCurSel(prop->GetCurrentIndex());
				}
				else
				{
					std::vector<std::string> names;
					const char *filter = prop->GetType() == ObjectProperty::TYPE_SKIN ? "skin/" : nullptr;
					texman.GetTextureNames(names, filter);
					for( auto &name: names )
					{
						// only allow using textures which are less than half of a cell
						const LogicalTexture &lt = texman.GetSpriteInfo(texman.FindSprite(name));
						if( lt.pxFrameWidth <= LOCATION_SIZE / 2 && lt.pxFrameHeight <= LOCATION_SIZE / 2 )
						{
							int index = static_cast<DefaultComboBox *>(ctrl.get())->GetData()->AddItem(name);
							if (name == prop->GetStringValue())
								static_cast<DefaultComboBox*>(ctrl.get())->SetCurSel(index);
						}
					}
				}
				break;
			default:
				assert(false);
			} // end of switch( prop->GetType() )

			if (!labelTextBuffer.str().empty())
			{
				auto typeRange = std::make_shared<UI::Text>(GetManager(), texman);
				typeRange->SetText(std::make_shared<UI::StaticText>(labelTextBuffer.str()));
				group->AddFront(typeRange);
			}

			group->AddFront(ctrl);
			group->SetFocus(ctrl);

			if( focus == i )
			{
				if(auto edit = std::dynamic_pointer_cast<UI::Edit>(ctrl) )
				{
					edit->SetSel(0, -1);
				}
				_psheet->SetFocus(group);
			}

			assert(nullptr != ctrl);
			_ctrls.push_back(ctrl);
		}
	}
}

void PropertyList::ConnectTo(std::shared_ptr<PropertySet> ps, TextureManager &texman)
{
	if( _ps == ps ) return;
	_ps = std::move(ps);
	DoExchange(false, texman);
}

FRECT PropertyList::GetChildRect(TextureManager &texman, const UI::LayoutContext &lc, const UI::StateContext &sc, const UI::Window &child) const
{
	if (_deleteButton.get() == &child)
	{
		return MakeRectWH(lc.GetPixelSize().x, _deleteButton->GetContentSize(texman, sc, lc.GetScale()).y);
	}
	if (_scrollView.get() == &child)
	{
		vec2d pxMargins = { std::floor(4 * lc.GetScale()), 1 };
		return MakeRectRB(pxMargins + vec2d{0, _deleteButton->GetContentSize(texman, sc, lc.GetScale()).y}, lc.GetPixelSize() - pxMargins);
	}
	return UI::Dialog::GetChildRect(texman, lc, sc, child);
}

bool PropertyList::OnKeyPressed(UI::InputContext &ic, UI::Key key)
{
	switch(key)
	{
	case UI::Key::Enter:
		DoExchange(true, GetManager().GetTextureManager());
		SaveToConfig(_conf, *_ps);
		break;
	case UI::Key::Escape:
		_conf.ed_showproperties.Set(false);
		SetVisible(false);
		break;
	default:
		return Dialog::OnKeyPressed(ic, key);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

void SaveToConfig(ShellConfig &conf, const PropertySet &ps)
{
	VariableTable &op = conf.ed_objproperties.GetTable(RTTypes::Inst().GetTypeInfo(ps.GetObject()->GetType()).name);
	for (int i = 0; i < ps.GetCount(); ++i)
	{
		const ObjectProperty *prop = const_cast<PropertySet&>(ps).GetProperty(i);
		switch (prop->GetType())
		{
		case ObjectProperty::TYPE_INTEGER:
			op.SetNum(prop->GetName(), prop->GetIntValue());
			break;
		case ObjectProperty::TYPE_FLOAT:
			op.SetNum(prop->GetName(), prop->GetFloatValue());
			break;
		case ObjectProperty::TYPE_STRING:
		case ObjectProperty::TYPE_SKIN:
		case ObjectProperty::TYPE_TEXTURE:
			op.SetStr(prop->GetName(), prop->GetStringValue());
			break;
		case ObjectProperty::TYPE_MULTISTRING:
			op.SetNum(prop->GetName(), (int)prop->GetCurrentIndex());
			break;
		default:
			assert(false);
		}
	}
}

void LoadFromConfig(const ShellConfig &conf, PropertySet &ps)
{
	VariableTable &op = conf.ed_objproperties.GetTable(RTTypes::Inst().GetTypeInfo(ps.GetObject()->GetType()).name);
	for (int i = 0; i < ps.GetCount(); ++i)
	{
		ObjectProperty *prop = ps.GetProperty(i);
		switch (prop->GetType())
		{
		case ObjectProperty::TYPE_INTEGER:
			prop->SetIntValue(std::min(prop->GetIntMax(),
				std::max(prop->GetIntMin(),
					op.GetNum(prop->GetName(), prop->GetIntValue()).GetInt())));
			break;
		case ObjectProperty::TYPE_FLOAT:
			prop->SetFloatValue(std::min(prop->GetFloatMax(),
				std::max(prop->GetFloatMin(),
					op.GetNum(prop->GetName(), prop->GetFloatValue()).GetFloat())));
			break;
		case ObjectProperty::TYPE_STRING:
		case ObjectProperty::TYPE_SKIN:
		case ObjectProperty::TYPE_TEXTURE:
			prop->SetStringValue(op.GetStr(prop->GetName(), prop->GetStringValue().c_str()).Get());
			break;
		case ObjectProperty::TYPE_MULTISTRING:
			prop->SetCurrentIndex(std::min((int)prop->GetListSize() - 1,
				std::max(0, op.GetNum(prop->GetName(), (int)prop->GetCurrentIndex()).GetInt())));
			break;
		default:
			assert(false);
		}
	}
}

