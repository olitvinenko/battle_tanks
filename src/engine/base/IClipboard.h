#pragma once

#include <string>

struct IClipboard
{
	virtual ~IClipboard() = default;

	virtual std::string GetText() = 0;
	virtual void SetText(const std::string& text) = 0;
};
