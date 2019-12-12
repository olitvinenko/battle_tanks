#pragma once

#include <vector>
#include <memory>

#include "base/IImage.h"

namespace FileSystem
{
    class Memory;
}

class TgaImage : public IImage
{
public:
	explicit TgaImage(std::shared_ptr<FileSystem::Memory> file);
    TgaImage(const void* data, unsigned long size);

	// Image methods
    const void* GetData() const override { return &m_data[0]; }
    uint8 GetBitsPerPixel() const override { return m_bpp; }
    uint32 GetWidth() const override { return m_width; }
    uint32 GetHeight() const override { return m_height; }

private:
	uint16 m_height;
	uint16 m_width;
	uint8 m_bpp;
	std::vector<int8> m_data;
};
