#pragma once

#include "VariableBase.h"

struct lua_State;

class VariableString : public VariableBase
{
public:
	VariableString();
	virtual ~VariableString();

	const std::string& Get() const;
	void Set(std::string value);

	// ConfVar
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};
