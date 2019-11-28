#include "VariableNumber.h"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <cassert>
#include <stdexcept>


VariableNumber::VariableNumber()
{
	m_type = typeNumber;
	m_val.asNumber = 0;
}

VariableNumber::~VariableNumber()
{
	m_type = typeNil;
}

double VariableNumber::GetRawNumber() const
{
	assert(m_type == typeNumber);
	return m_val.asNumber;
}

void VariableNumber::SetRawNumber(double value)
{
	assert(m_type == typeNumber);
	if( m_val.asNumber != value )
	{
		m_val.asNumber = value;
		FireValueUpdate(this);
	}
}

float VariableNumber::GetFloat() const
{
	assert(m_type == typeNumber);
	return (float) m_val.asNumber;
}
void VariableNumber::SetFloat(float value)
{
	assert(m_type == typeNumber);
	if( m_val.asNumber != value )
	{
		m_val.asNumber = value;
		FireValueUpdate(this);
	}
}

int VariableNumber::GetInt() const
{
	assert(m_type == typeNumber);
	return (int) m_val.asNumber;
}
void VariableNumber::SetInt(int value)
{
	assert(m_type == typeNumber);
	if( m_val.asNumber != value )
	{
		m_val.asNumber = value;
		FireValueUpdate(this);
	}
}

bool VariableNumber::Write(FILE *file, int indent) const
{
	fprintf(file, "%.10g", m_val.asNumber);
	return true;
}

bool VariableNumber::Assign(lua_State *L)
{
	SetFloat( (float) lua_tonumber(L, -1) );
	return true;
}

void VariableNumber::Push(lua_State *L) const
{
	lua_pushnumber(L, GetFloat());
}
