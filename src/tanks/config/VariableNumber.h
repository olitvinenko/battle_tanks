#pragma once

#include "VariableBase.h"

struct lua_State;

class VariableNumber : public VariableBase
{
public:
	VariableNumber();
	virtual ~VariableNumber();

	double GetRawNumber() const;
	void SetRawNumber(double value);

	float GetFloat() const;
	void SetFloat(float value);

	int GetInt() const;
	void SetInt(int value);

	// ConfVar
    void Push(lua_State *L) const override;
    bool Assign(lua_State *L) override;
    bool Write(FILE *file, int indent) const override;
};