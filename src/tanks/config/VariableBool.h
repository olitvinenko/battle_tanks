#pragma once

#include "VariableBase.h"

struct lua_State;

class VariableBool final : public VariableBase
{
public:
	VariableBool();
    ~VariableBool();

	bool Get() const;
	void Set(bool value);

	// ConfVar
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};
