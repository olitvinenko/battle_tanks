#pragma once

#include "common/Types.h"

class IImage
{
public:
	virtual ~IImage() = default;

	virtual const uint8* GetData() const = 0;
	virtual uint8 GetBitsPerPixel() const = 0;
	virtual uint32 GetWidth() const = 0;
	virtual uint32 GetHeight() const = 0;
};
