#include "VariableBase.h"

#include "VariableBool.h"
#include "VariableNumber.h"
#include "VariableString.h"
#include "VariableArray.h"
#include "VariableTable.h"
#include "VariableNil.h"

extern "C"
{
    #include <lua.h>
}

#include <cassert>

namespace config_detail
{
	VariableBase* GetVarIfTypeMatch(VariableTable* parent, const char* key, VariableBase::Type type)
	{
		if (VariableBase* v = parent->Find(key))
		{
			return v->GetType() == type ? v : nullptr;
		}

		return parent->GetVar(key, type).first;
	}

	VariableBase* TableElementFromLua(lua_State* L, VariableTable* parent, const char* key)
	{
		VariableBase* result = nullptr;
		int valueType = lua_type(L, -1);

		switch (valueType)
		{
		case LUA_TSTRING:
			result = GetVarIfTypeMatch(parent, key, VariableBase::STRING);
			break;
		case LUA_TBOOLEAN:
			result = GetVarIfTypeMatch(parent, key, VariableBase::BOOLEAN);
			break;
		case LUA_TNUMBER:
			result = GetVarIfTypeMatch(parent, key, VariableBase::NUMBER);
			break;
		case LUA_TTABLE:
			result = GetVarIfTypeMatch(parent, key, lua_objlen(L, -1) ? VariableBase::ARRAY : VariableBase::TABLE);
			break;

		default:
			return nullptr;
		}

		return result;
	}

	VariableBase* ArrayElementFromLua(lua_State* L, VariableArray* parent, size_t key)
	{
		VariableBase* result = nullptr;
		int valueType = lua_type(L, -1);

		switch (valueType)
		{
		case LUA_TSTRING:
			result = parent->GetVar(key, VariableBase::STRING).first;
			break;
		case LUA_TBOOLEAN:
			result = parent->GetVar(key, VariableBase::BOOLEAN).first;
			break;
		case LUA_TNUMBER:
			result = parent->GetVar(key, VariableBase::NUMBER).first;
			break;
		case LUA_TTABLE:
			result = parent->GetVar(key, lua_objlen(L, -1) ? VariableBase::ARRAY : VariableBase::TABLE).first;
			break;

		default:
			return nullptr;
		}

		return result;
	}
}

VariableBase::VariableBase()
  : m_type(NIL)
{
}

VariableBase::~VariableBase()
{
	assert(m_type == NIL);
}

void VariableBase::FireValueUpdate(VariableBase *pVar)
{
	if( eventChange )
		eventChange();
}


void VariableBase::SetType(Type type)
{
    if (m_type == type)
        return;
    
    this->~VariableBase(); // manually call the destructor
        
    switch( type )
    {
#pragma push_macro("new")
#undef new
        case NIL:     new(this) VariableNil();    break;
        case NUMBER:  new(this) VariableNumber();  break;
        case BOOLEAN: new(this) VariableBool();    break;
        case STRING:  new(this) VariableString();  break;
        case ARRAY:   new(this) VariableArray();   break;
        case TABLE:   new(this) VariableTable();   break;
        default: assert(!"unknown ConfVar type");
#pragma pop_macro("new")
    }
    assert( m_type == type );
}

VariableNumber& VariableBase::AsNum()
{
	assert(m_type == NUMBER);
	return static_cast<VariableNumber&>(*this);
}

VariableBool& VariableBase::AsBool()
{
	assert(m_type == BOOLEAN);
	return static_cast<VariableBool&>(*this);
}

VariableString& VariableBase::AsStr()
{
	assert(m_type == STRING);
	return static_cast<VariableString&>(*this);
}

VariableArray& VariableBase::AsArray()
{
	assert(m_type == ARRAY);
	return static_cast<VariableArray&>(*this);
}

VariableTable& VariableBase::AsTable()
{
	assert(m_type == TABLE);
	return static_cast<VariableTable&>(*this);
}
