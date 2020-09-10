#pragma once

#include "VariableBase.h"

struct lua_State;

class VariableNil final : public VariableBase
{
public:
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};
