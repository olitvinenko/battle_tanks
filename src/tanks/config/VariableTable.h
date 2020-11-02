#pragma once

#include "VariableBase.h"

#include <vector>

class VariableNumber;
class VariableBool;
class VariableString;
class VariableArray;

struct lua_State;

class VariableTable final : public VariableBase
{
public:
	VariableTable();
    ~VariableTable();

	VariableBase* Find(const std::string& name); // returns nullptr if variable not found
	size_t GetSize() const;

	std::vector<std::string> GetKeys() const;

	// bool part contains true if value with the specified type was found
	std::pair<VariableBase*, bool> GetVar(const std::string &name, VariableBase::Type type);

	VariableNumber& GetNum(const std::string& name, float def);
	VariableNumber& GetNum(const std::string& name, int   def = 0);
	VariableBool&   GetBool(const std::string& name, bool  def = false);
	VariableString& GetStr(const std::string& name);
	VariableString& GetStr(const std::string& name, std::string def);

	VariableNumber& SetNum(const std::string& name, float value);
	VariableNumber& SetNum(const std::string& name, int   value);
	VariableBool&   SetBool(const std::string& name, bool  value);
	VariableString& SetStr(const std::string& name, std::string value);

	VariableArray& GetArray(const std::string& name, void (*init)(VariableArray&) = nullptr);
	VariableTable& GetTable(const std::string& name, void (*init)(VariableTable&) = nullptr);

	void Clear();
    
	bool Remove(const VariableBase& value);
	bool Remove(const std::string& name);
    
	bool Rename(const VariableBase& value, std::string newName);
	bool Rename(const std::string& oldName, std::string newName);

	bool Save(const char* filename) const;
	bool Load(const char* filename);

protected:
    void Push(lua_State* L) const override;
    bool Assign(lua_State* L) override;
    bool Write(FILE* file, int indent) const override;
};
