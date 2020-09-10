#pragma once

#include "VariableBase.h"

struct lua_State;

class VariableString final : public VariableBase
{
public:
	VariableString();
    ~VariableString();

	const std::string& Get() const;
	void Set(std::string value);

	// ConfVar
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};
