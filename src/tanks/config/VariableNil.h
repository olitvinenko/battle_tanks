#pragma once

#include "VariableBase.h"

struct lua_State;

class VariableNil final : public VariableBase
{
protected:
    void Push(lua_State*) const override;
    bool Assign(lua_State*) override;
    bool Write(FILE* file, int indent) const override;
};
