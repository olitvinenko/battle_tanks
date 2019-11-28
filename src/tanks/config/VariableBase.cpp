#include "VariableBase.h"

#include "VariableBool.h"
#include "VariableNumber.h"
#include "VariableString.h"
#include "VariableArray.h"
#include "VariableTable.h"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <cassert>
#include <stdexcept>

VariableBase* GetVarIfTypeMatch(VariableTable *parent, const char *key, VariableBase::Type type)
{
	if( VariableBase *v = parent->Find(key) )
	{
		return v->GetType() == type ? v : nullptr;
	}

	return parent->GetVar(key, type).first;
}

VariableBase* TableElementFromLua(lua_State *L, VariableTable *parent, const char *key)
{
	VariableBase* result = nullptr;
	int valueType = lua_type(L, -1);
    
	switch( valueType )
	{
	case LUA_TSTRING:
		result = GetVarIfTypeMatch(parent, key, VariableBase::typeString);
		break;
	case LUA_TBOOLEAN:
		result = GetVarIfTypeMatch(parent, key, VariableBase::typeBoolean);
		break;
	case LUA_TNUMBER:
		result = GetVarIfTypeMatch(parent, key, VariableBase::typeNumber);
		break;
	case LUA_TTABLE:
		result = GetVarIfTypeMatch(parent, key, lua_objlen(L,-1) ? VariableBase::typeArray : VariableBase::typeTable);
		break;
            
	default:
		return nullptr;
	}

	return result;
}

VariableBase* ArrayElementFromLua(lua_State *L, VariableArray *parent, size_t key)
{
	VariableBase* result = nullptr;
	int valueType = lua_type(L, -1);
    
	switch( valueType )
	{
	case LUA_TSTRING:
		result = parent->GetVar(key, VariableBase::typeString).first;
		break;
	case LUA_TBOOLEAN:
		result = parent->GetVar(key, VariableBase::typeBoolean).first;
		break;
	case LUA_TNUMBER:
		result = parent->GetVar(key, VariableBase::typeNumber).first;
		break;
	case LUA_TTABLE:
		result = parent->GetVar(key, lua_objlen(L,-1) ? VariableBase::typeArray : VariableBase::typeTable).first;
		break;
            
	default:
        return nullptr;
	}

	return result;
}

VariableBase::VariableBase()
  : m_type(typeNil)
{
}

VariableBase::~VariableBase()
{
	assert(m_type == typeNil);
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
        case typeNil:     new(this) VariableBase();    break;
        case typeNumber:  new(this) VariableNumber();  break;
        case typeBoolean: new(this) VariableBool();    break;
        case typeString:  new(this) VariableString();  break;
        case typeArray:   new(this) VariableArray();   break;
        case typeTable:   new(this) VariableTable();   break;
        default: assert(!"unknown ConfVar type");
#pragma pop_macro("new")
    }
    assert( m_type == type );
}

VariableNumber& VariableBase::AsNum()
{
	assert(m_type == typeNumber);
	return static_cast<VariableNumber&>(*this);
}

VariableBool& VariableBase::AsBool()
{
	assert(m_type == typeBoolean);
	return static_cast<VariableBool&>(*this);
}

VariableString& VariableBase::AsStr()
{
	assert(m_type == typeString);
	return static_cast<VariableString&>(*this);
}

VariableArray& VariableBase::AsArray()
{
	assert(m_type == typeArray);
	return static_cast<VariableArray&>(*this);
}

VariableTable& VariableBase::AsTable()
{
	assert(m_type == typeTable);
	return static_cast<VariableTable&>(*this);
}

bool VariableBase::Write(FILE *file, int indent) const
{
	fprintf(file, "nil");
	return true;
}

bool VariableBase::Assign(lua_State *)
{
	assert(false);
	return false;
}

void VariableBase::Push(lua_State *) const
{
	assert(false);
}
