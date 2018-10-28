#pragma once

#include <string>

struct IClipboard
{
	virtual ~IClipboard() = default;

	virtual const std::string& GetText() = 0;
	virtual void SetText(const std::string& text) = 0;
};
