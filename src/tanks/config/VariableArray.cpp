#include "VariableArray.h"

#include "VariableBool.h"
#include "VariableNumber.h"
#include "VariableString.h"
#include "VariableTable.h"
#include "VariableNil.h"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <cassert>
#include <stdexcept>

VariableArray::VariableArray()
{
	m_type = typeArray;
	m_val.asArray = new std::deque<std::unique_ptr<VariableBase>>();
}

VariableArray::~VariableArray()
{
	assert(m_type == typeArray);
	delete m_val.asArray;
	m_type = typeNil;
}

std::pair<VariableBase*, bool> VariableArray::GetVar(size_t index, VariableBase::Type type)
{
	assert( index < GetSize() );
	std::pair<VariableBase*, bool> result( (*m_val.asArray)[index].get(), true);

	if( result.first->GetType() != type )
	{
		result.first->SetType(type);
		result.second = false;
		FireValueUpdate(this);
		assert(result.first->GetType() == type);
	}

	return result;
}

VariableNumber& VariableArray::GetNum(size_t index, float def)
{
	std::pair<VariableBase*, bool> p = GetVar(index, VariableBase::typeNumber);
	if( !p.second )
		p.first->AsNum().SetFloat(def);
	return p.first->AsNum();
}

VariableNumber& VariableArray::SetNum(size_t index, float value)
{
	VariableNumber &v = GetVar(index, VariableBase::typeNumber).first->AsNum();
	v.SetFloat(value);
	return v;
}

VariableNumber& VariableArray::GetNum(size_t index, int def)
{
	std::pair<VariableBase*, bool> p = GetVar(index, VariableBase::typeNumber);
	if( !p.second )
		p.first->AsNum().SetInt(def);
	return p.first->AsNum();
}

VariableNumber& VariableArray::SetNum(size_t index, int value)
{
	VariableNumber &v = GetVar(index, VariableBase::typeNumber).first->AsNum();
	v.SetInt(value);
	return v;
}

VariableBool& VariableArray::GetBool(size_t index, bool def)
{
	std::pair<VariableBase*, bool> p = GetVar(index, VariableBase::typeBoolean);
	if( !p.second )
		p.first->AsBool().Set(def);
	return p.first->AsBool();
}

VariableBool& VariableArray::SetBool(size_t index, bool value)
{
	VariableBool &v = GetVar(index, VariableBase::typeBoolean).first->AsBool();
	v.Set(value);
	return v;
}

VariableString& VariableArray::GetStr(size_t index)
{
	return GetVar(index, VariableBase::typeString).first->AsStr();
}

VariableString& VariableArray::GetStr(size_t index, std::string def)
{
	std::pair<VariableBase*, bool> p = GetVar(index, VariableBase::typeString);
	if (!p.second)
		p.first->AsStr().Set(std::move(def));
	return p.first->AsStr();
}

VariableString& VariableArray::SetStr(size_t index, std::string value)
{
	VariableString &v = GetVar(index, VariableBase::typeString).first->AsStr();
	v.Set(std::move(value));
	return v;
}

VariableArray& VariableArray::GetArray(size_t index)
{
	return GetVar(index, VariableBase::typeArray).first->AsArray();
}

VariableTable& VariableArray::GetTable(size_t index)
{
	return GetVar(index, VariableBase::typeTable).first->AsTable();
}

void VariableArray::EnsureIndex(size_t index)
{
	if (index + 1 > GetSize())
		Resize(index + 1);
}

void VariableArray::Resize(size_t newSize)
{
	assert(m_type == typeArray);
	size_t oldSize = m_val.asArray->size();
	if (newSize != oldSize)
	{
		m_val.asArray->resize(newSize);
		if (newSize > oldSize)
		{
			for (size_t i = oldSize; i < newSize; ++i)
			{
				(*m_val.asArray)[i].reset(new VariableNil());
			}
		}
		FireValueUpdate(this);
	}
}

size_t VariableArray::GetSize() const
{
	assert(m_type == typeArray);
	return m_val.asArray->size();
}

VariableBase& VariableArray::GetAt(size_t index) const
{
	assert(m_type == typeArray);
	return *(*m_val.asArray)[index];
}

void VariableArray::RemoveAt(size_t index)
{
	assert(m_type == typeArray);
	m_val.asArray->erase(m_val.asArray->begin() + index);
	FireValueUpdate(this);
}

void VariableArray::PopFront()
{
	assert(m_type == typeArray);
	m_val.asArray->pop_front();
	FireValueUpdate(this);
}

VariableBase& VariableArray::PushBack(Type type)
{
	assert(m_type == typeArray);
	std::unique_ptr<VariableBase> result(new VariableNil());
	result->SetType(type);
	m_val.asArray->push_back(std::move(result));
	FireValueUpdate(this);
	return *m_val.asArray->back();
}

void VariableArray::PopBack()
{
	assert(m_type == typeArray);
	m_val.asArray->pop_back();
	FireValueUpdate(this);
}

VariableBase& VariableArray::PushFront(Type type)
{
	assert(m_type == typeArray);
	std::unique_ptr<VariableBase> result(new VariableNil());
	result->SetType(type);
	m_val.asArray->push_front(std::move(result));
	FireValueUpdate(this);
	return *m_val.asArray->front();
}

bool VariableArray::Write(FILE *file, int indent) const
{
	fprintf(file, "{\n");
	bool delim = false;
	for( size_t i = 0; i < GetSize(); ++i )
	{
		if( delim ) fprintf(file, ",\n");
		for( int n = 0; n < indent; ++n )
			fprintf(file, "  ");
		delim = true;
		if( !GetAt(i).Write(file, indent+1) )
			return false;
	}
	fprintf(file, "\n");
	for (int n = 0; n < indent - 1; ++n)
		fprintf(file, "  ");
	fprintf(file, "}");
	return true;
}

bool VariableArray::Assign(lua_State *L)
{
	Resize( lua_objlen(L, -1) );

	for( size_t i = 0; i < GetSize(); ++i )
	{
		lua_pushinteger(L, i+1); // push the key
		lua_gettable(L, -2);   // pop the key, push the value

		if( VariableBase *v = ArrayElementFromLua(L, this, i) )
		{
			if( !v->Assign(L) )
				return false;
		}

		lua_pop(L, 1);         // pop the value
	}

	return true;
}

void VariableArray::Push(lua_State *L) const
{
	*reinterpret_cast<VariableArray const**>( lua_newuserdata(L, sizeof(this)) ) = this;
	luaL_getmetatable(L, "conf_array");  // metatable for config
	lua_setmetatable(L, -2);
}
