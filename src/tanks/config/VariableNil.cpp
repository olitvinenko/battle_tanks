#include "VariableNil.h"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <cassert>

bool VariableNil::Write(FILE *file, int indent) const
{
    fprintf(file, "nil");
    return true;
}

bool VariableNil::Assign(lua_State *)
{
    assert(false);
    return false;
}

void VariableNil::Push(lua_State *) const
{
    assert(false);
}
