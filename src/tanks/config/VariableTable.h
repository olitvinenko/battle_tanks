#pragma once

#include "VariableBase.h"

class VariableNumber;
class VariableBool;
class VariableString;
class VariableArray;

struct lua_State;

class VariableTable : public VariableBase
{
public:
	VariableTable();
	virtual ~VariableTable();

	VariableBase* Find(const std::string &name); // returns nullptr if variable not found
	size_t GetSize() const;

	typedef std::vector<std::string> KeyListType;
	KeyListType GetKeys() const;

	// bool part contains true if value with the specified type was found
	std::pair<VariableBase*, bool> GetVar(const std::string &name, VariableBase::Type type);

	VariableNumber& GetNum(std::string name, float def);
	VariableNumber& GetNum(std::string name, int   def = 0);
	VariableBool&  GetBool(std::string name, bool  def = false);
	VariableString& GetStr(std::string name);
	VariableString& GetStr(std::string name, std::string def);

	VariableNumber& SetNum(std::string name, float value);
	VariableNumber& SetNum(std::string name, int   value);
	VariableBool&  SetBool(std::string name, bool  value);
	VariableString& SetStr(std::string name, std::string value);

	VariableArray& GetArray(std::string name, void (*init)(VariableArray&) = nullptr);
	VariableTable& GetTable(std::string name, void (*init)(VariableTable&) = nullptr);

	void Clear();
    
	bool Remove(const VariableBase &value);
	bool Remove(const std::string &name);
    
	bool Rename(const VariableBase &value, std::string newName);
	bool Rename(const std::string &oldName, std::string newName);

	bool Save(const char *filename) const;
	bool Load(const char *filename);

	// ConfVar
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};
