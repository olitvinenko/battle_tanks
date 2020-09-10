#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <deque>
#include <vector>

class VariableBool;
class VariableNumber;
class VariableString;
class VariableArray;
class VariableTable;

struct lua_State;

class VariableBase
{
public:
	enum Type
	{
		typeNil,
		typeNumber,
		typeBoolean,
		typeString,
		typeArray,
		typeTable,
	};

	VariableBase();
	virtual ~VariableBase();

	void SetType(Type type);
	Type GetType() const { return m_type; }

	// type casting
    VariableBool&         AsBool();
	VariableNumber&       AsNum();
	VariableString&       AsStr();
	VariableArray&        AsArray();
	VariableTable&        AsTable();

	// lua binding helpers
    virtual void Push(lua_State *L) const = 0;
    virtual bool Assign(lua_State *L) = 0;
    // serialization
    virtual bool Write(FILE *file, int indent) const = 0;
    
	// notifications
	std::function<void(void)> eventChange;

protected:
	void FireValueUpdate(VariableBase *pVar);

	using TableType = std::map<std::string, std::unique_ptr<VariableBase>>;
    using ArrayType = std::deque<std::unique_ptr<VariableBase>>;

	union Value
	{
		double        asNumber;
		bool          asBool;
		std::string   *asString;
		ArrayType     *asArray;
		TableType     *asTable;
	};

	Type  m_type;
	Value m_val;
};


VariableBase* GetVarIfTypeMatch(VariableTable *parent, const char *key, VariableBase::Type type);
VariableBase* TableElementFromLua(lua_State *L, VariableTable *parent, const char *key);
VariableBase* ArrayElementFromLua(lua_State *L, VariableArray *parent, size_t key);
