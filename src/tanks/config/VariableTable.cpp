
#include "VariableTable.h"

#include "VariableBool.h"
#include "VariableNumber.h"
#include "VariableArray.h"
#include "VariableString.h"
#include "VariableNil.h"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <cassert>
#include <stdexcept>


VariableTable::VariableTable()
{
	m_type = typeTable;
	m_val.asTable = new std::map<std::string, std::unique_ptr<VariableBase>>();
}

VariableTable::~VariableTable()
{
	assert(m_type == typeTable);
	delete m_val.asTable;
	m_type = typeNil;
}

VariableBase* VariableTable::Find(const std::string &name)  // returns nullptr if variable not found
{
	auto it = m_val.asTable->find(name);
	return m_val.asTable->end() != it ? it->second.get() : nullptr;
}

size_t VariableTable::GetSize() const
{
	return m_val.asTable->size();
}

std::vector<std::string> VariableTable::GetKeys() const
{
	std::vector<std::string> out;
	for( auto it = m_val.asTable->begin(); m_val.asTable->end() != it; ++it )
	{
		out.push_back(it->first);
	}
	return out;
}

std::pair<VariableBase*, bool> VariableTable::GetVar(const std::string &name, VariableBase::Type type)
{
	std::pair<VariableBase*, bool> result(nullptr, true);

	assert(m_type != typeNil);
	auto it = m_val.asTable->find(name);
	if( m_val.asTable->end() == it )
	{
		// create new item
		result.first = m_val.asTable->emplace(std::move(name), std::unique_ptr<VariableBase>(new VariableNil())).first->second.get();
		FireValueUpdate(this);
	}
	else
	{
		result.first = it->second.get();
	}

	if( result.first->GetType() != type )
	{
		// change type of the existing item
		result.first->SetType(type);
		result.second = false;
		FireValueUpdate(this);
	}
	assert(result.first->GetType() == type);

	return result;
}

VariableNumber& VariableTable::GetNum(std::string name, float def)
{
	std::pair<VariableBase*, bool> p = GetVar(std::move(name), VariableBase::typeNumber);
	if( !p.second )
		p.first->AsNum().SetFloat(def);
	return p.first->AsNum();
}

VariableNumber& VariableTable::SetNum(std::string name, float value)
{
	VariableNumber &v = GetVar(std::move(name), VariableBase::typeNumber).first->AsNum();
	v.SetFloat(value);
	return v;
}

VariableNumber& VariableTable::GetNum(std::string name, int def)
{
	std::pair<VariableBase*, bool> p = GetVar(std::move(name), VariableBase::typeNumber);
	if( !p.second )
		p.first->AsNum().SetInt(def);
	return p.first->AsNum();
}

VariableNumber& VariableTable::SetNum(std::string name, int value)
{
	VariableNumber &v = GetVar(std::move(name), VariableBase::typeNumber).first->AsNum();
	v.SetInt(value);
	return v;
}

VariableBool& VariableTable::GetBool(std::string name, bool def)
{
	std::pair<VariableBase*, bool> p = GetVar(std::move(name), VariableBase::typeBoolean);
	if( !p.second )
		p.first->AsBool().Set(def);
	return p.first->AsBool();
}

VariableBool& VariableTable::SetBool(std::string name, bool value)
{
	VariableBool &v = GetVar(std::move(name), VariableBase::typeBoolean).first->AsBool();
	v.Set(value);
	return v;
}

VariableString& VariableTable::GetStr(std::string name)
{
	return GetVar(std::move(name), VariableBase::typeString).first->AsStr();
}

VariableString& VariableTable::GetStr(std::string name, std::string def)
{
	std::pair<VariableBase*, bool> p = GetVar(std::move(name), VariableBase::typeString);
	if (!p.second)
		p.first->AsStr().Set(std::move(def));
	return p.first->AsStr();
}

VariableString& VariableTable::SetStr(std::string name, std::string value)
{
	VariableString &v = GetVar(std::move(name), VariableBase::typeString).first->AsStr();
	v.Set(std::move(value));
	return v;
}

VariableArray& VariableTable::GetArray(std::string name, void (*init)(VariableArray&))
{
	std::pair<VariableBase*, bool> p = GetVar(std::move(name), VariableBase::typeArray);
	if( !p.second && init )
		init(p.first->AsArray());
	return p.first->AsArray();
}

VariableTable& VariableTable::GetTable(std::string name, void (*init)(VariableTable&))
{
	std::pair<VariableBase*, bool> p = GetVar(std::move(name), VariableBase::typeTable);
	if( !p.second && init )
		init(p.first->AsTable());
	return p.first->AsTable();
}

void VariableTable::Clear()
{
	assert(m_type == typeTable);

	m_val.asTable->clear();
	FireValueUpdate(this);
}

bool VariableTable::Remove(const VariableBase& value)
{
	assert(m_type == typeTable);

	for( auto it = m_val.asTable->begin(); m_val.asTable->end() != it; ++it )
	{
		if( &value == it->second.get() )
		{
			m_val.asTable->erase(it);
			FireValueUpdate(this);
			return true;
		}
	}
	return false;
}

bool VariableTable::Remove(const std::string &name)
{
	assert(m_type == typeTable);

	auto it = m_val.asTable->find(name);
	if( m_val.asTable->end() != it )
	{
		m_val.asTable->erase(it);
		FireValueUpdate(this);
		return true;
	}
	return false;
}

bool VariableTable::Rename(const VariableBase& value, std::string newName)
{
	assert(m_type == typeTable);

	auto it = m_val.asTable->begin();
	while( m_val.asTable->end() != it )
	{
		if( &value == it->second.get() )
		{
			break;
		}
		++it;
	}
	if( m_val.asTable->end() == it )
	{
		return false; // old name not found
	}

	if( it->first == newName )
	{
		return true;
	}

	if( m_val.asTable->count(newName) )
	{
		return false; // new name is already in use
	}

	m_val.asTable->emplace(std::move(newName), std::move(it->second));
	m_val.asTable->erase(it);

	FireValueUpdate(this);

	return true;
}

bool VariableTable::Rename(const std::string &oldName, std::string newName)
{
	assert(m_type == typeTable);

	auto it = m_val.asTable->find(oldName);
	if( m_val.asTable->end() == it )
	{
		return false; // old name not found
	}

	if( it->first == newName )
	{
		return true;
	}

	if( m_val.asTable->count(newName) )
	{
		return false; // new name is already in use
	}

	m_val.asTable->emplace(std::move(newName), std::move(it->second));
	m_val.asTable->erase(it);

	FireValueUpdate(this);

	return true;
}

bool VariableTable::Write(FILE *file, int indent) const
{
    if (indent)
        fprintf(file, "{\n");
    
	for( auto it = m_val.asTable->begin(); m_val.asTable->end() != it; ++it )
	{
		for( int i = 0; i < indent; ++i )
		{
			fputs("  ", file);
		}

		bool safe = true;
		for( size_t i = 0; i < it->first.size(); ++i )
		{
			unsigned char c = it->first[i];
			if( !isalpha(c) && '_' != c )
			{
				if( 0 == i || !isdigit(c) )
				{
					safe = false;
					break;
				}
			}
		}

		if( safe )
		{
			fputs(it->first.c_str(), file);
		}
		else
		{
			fputs("[\"", file);
			for( size_t i = 0; i < it->first.size(); ++i )
			{
				const int c = it->first[i];
				switch(c)
				{
				case '\\':
					fputs("\\\\", file);
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
			fputs("\"]", file);
		}

		fputs(" = ", file);

		if( !it->second->Write(file, indent+1) )
			return false;

		fputs(indent ? ",":";", file);
		fputs("\n", file);
	}
	if( indent )
	{
		for( int i = 0; i < indent-1; ++i )
		{
			fputs("  ", file);
		}
		fputs("}", file);
	}

	return true;
}

bool VariableTable::Assign(lua_State *L)
{
	// enumerate all fields of the table
	for( lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1) )
	{
		// now 'key' is at index -2 and 'value' at index -1
		// use only string keys
		if( LUA_TSTRING == lua_type(L, -2) )
		{
			const char *key = lua_tostring(L, -2);
			if( VariableBase* v = TableElementFromLua(L, this, key) )
			{
				if( !v->Assign(L) )
					return false;
			}
			else
			{
//				_logger.Printf(1, "variable '%s' was dropped", key);
			}
		}
	}

	return true;
}

bool VariableTable::Save(const char *filename) const
{
	FILE *file = fopen(filename, "w");
	if( !file )
	{
		return false;
	}
	fprintf(file, "-- config file was automatically generated by application\n\n");
	bool result = Write(file, 0);
	fclose(file);
	return result;
}

bool VariableTable::Load(const char *filename)
{
	lua_State *L = lua_open();

	// try to read and execute the file
	if( luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0) )
	{
        std::runtime_error error(lua_tostring(L, -1));
		lua_close(L);
        return false;
		//throw error;
	}

	// get global table
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	bool result = Assign(L);

	lua_close(L);

	FireValueUpdate(this);

	return result;
}

void VariableTable::Push(lua_State *L) const
{
	*reinterpret_cast<VariableTable const**>(lua_newuserdata(L, sizeof(this))) = this;
	luaL_getmetatable(L, "conf_table");  // metatable for config
	lua_setmetatable(L, -2);
}
