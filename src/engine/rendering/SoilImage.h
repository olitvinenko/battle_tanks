#pragma once

#include <memory>

#include "base/IImage.h"

namespace FileSystem
{
    class Memory;
}

class SoilImage : public IImage
{
public:
    SoilImage(std::shared_ptr<FileSystem::Memory> file);
    SoilImage(const void* data, unsigned long size);
    ~SoilImage();
    
	// Image methods
    const void* GetData() const override { return m_soilData; }
    uint8 GetBitsPerPixel() const override { return m_bpp; }
    uint32 GetWidth() const override { return m_width; }
    uint32 GetHeight() const override { return m_height; }

private:
	uint16 m_height;
	uint16 m_width;
	uint8 m_bpp;
    uint8* m_soilData;
};
