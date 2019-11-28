#include "VariableBool.h"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <cassert>
#include <stdexcept>

VariableBool::VariableBool()
{
	m_type = typeBoolean;
	m_val.asBool = false;
}

VariableBool::~VariableBool()
{
	m_type = typeNil;
}

bool VariableBool::Get() const
{
	assert(m_type == typeBoolean);
	return m_val.asBool;
}
void VariableBool::Set(bool value)
{
	assert(m_type == typeBoolean);
	m_val.asBool = value;
	FireValueUpdate(this);
}

bool VariableBool::Write(FILE *file, int indent) const
{
	fprintf(file, Get() ? "true" : "false");
	return true;
}

bool VariableBool::Assign(lua_State *L)
{
	Set( 0 != lua_toboolean(L, -1) );
	return true;
}

void VariableBool::Push(lua_State *L) const
{
	lua_pushboolean(L, Get());
}
