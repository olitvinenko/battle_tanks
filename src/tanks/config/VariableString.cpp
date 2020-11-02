#include "VariableString.h"

extern "C"
{
    #include <lua.h>
	#include <lauxlib.h>
}

#include <cassert>
#include <stdexcept>

VariableString::VariableString()
{
	m_type = STRING;
	m_val.asString = new std::string();
}

VariableString::~VariableString()
{
	assert(m_type == STRING);
	delete m_val.asString;
	m_type = NIL;
}

const std::string& VariableString::Get() const
{
	assert(m_type == STRING);
	return *m_val.asString;
}

void VariableString::Set(std::string value)
{
	assert(m_type == STRING);
	*m_val.asString = std::move(value);
	FireValueUpdate(this);
}

bool VariableString::Write(FILE* file, int indent) const
{
	fputc('"', file);
	for ( size_t i = 0; i < m_val.asString->size(); ++i )
	{
		const int c = (*m_val.asString)[i];
		switch(c)
		{
		case '\\':
			fputs("\\\\", file);
			break;
		case '\r':
			fputs("\\r", file);
			break;
		case '\n':
			fputs("\\n", file);
			break;
		case '"':
			fputs("\\\"", file);
			break;
		case '\t':
			fputs("\\t", file);
			break;
		default:
			fputc(c, file);
		}
	}
	fputc('"', file);
	return true;
}

bool VariableString::Assign(lua_State* L)
{
	Set(lua_tostring(L, -1));
	return true;
}

void VariableString::Push(lua_State* L) const
{
	lua_pushstring(L, Get().c_str());
}
