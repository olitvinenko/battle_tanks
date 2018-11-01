#pragma once

#include <vector>
#include <memory>

#include "IImage.h"
#include "FileSystem.h"

class TgaImage : public IImage
{
public:
	explicit TgaImage(std::shared_ptr<FileSystem::File::Memory> file);

	~TgaImage();

	// Image methods
	const void* GetData() const override;
	uint32 GetBitsPerPixel() const override;
	uint32 GetWidth() const override;
	uint32 GetHeight() const override;

private:
	uint32 _height;
	uint32 _width;
	uint32 _bpp;
	std::vector<int8_t> _data;
};


// end of file
