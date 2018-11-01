#pragma once

#include "Types.h"

class IImage
{
public:
	virtual ~IImage() = default;

	virtual const void* GetData() const = 0;
	virtual uint32 GetBitsPerPixel() const = 0;
	virtual uint32 GetWidth() const = 0;
	virtual uint32 GetHeight() const = 0;
};
