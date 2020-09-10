#pragma once

#include "VariableBase.h"

class VariableNumber;
class VariableBool;
class VariableString;
class VariableTable;

class VariableArray final : public VariableBase
{
public:
	VariableArray();
    ~VariableArray();

	// bool part contains true if value with the specified type was found
	std::pair<VariableBase*, bool> GetVar(size_t index, VariableBase::Type type);

	VariableNumber& GetNum(size_t index, float def);
	VariableNumber& GetNum(size_t index, int   def = 0);
	VariableBool&   GetBool(size_t index, bool  def = false);
	VariableString& GetStr(size_t index);
	VariableString& GetStr(size_t index, std::string def);

	VariableNumber& SetNum(size_t index, float value);
	VariableNumber& SetNum(size_t index, int   value);
	VariableBool&   SetBool(size_t index, bool  value);
	VariableString& SetStr(size_t index, std::string value);

	VariableArray&  GetArray(size_t index);
	VariableTable&  GetTable(size_t index);

	void      EnsureIndex(size_t index);
	void      Resize(size_t newSize);
	size_t    GetSize() const;

	VariableBase&  GetAt(size_t index) const;
	void      RemoveAt(size_t index);

	void      PopFront();
	void      PopBack();
	VariableBase&  PushFront(Type type);
	VariableBase&  PushBack(Type type);

	// ConfVar
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};
