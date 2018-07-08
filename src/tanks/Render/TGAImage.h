#pragma once

#include <vector>
#include <memory>

#include "RenderBase.h"
#include "FileSystem.h"


class TgaImage : public Image
{
public:
	explicit TgaImage(std::shared_ptr<FileSystem::File::Memory> file);

	~TgaImage();

	// Image methods
	const void* GetData() const override;
	unsigned int GetBitsPerPixel() const override;
	unsigned int GetWidth() const override;
	unsigned int GetHeight() const override;

private:
	unsigned int _height;
	unsigned int _width;
	unsigned int _bpp;
	std::vector<char> _data;
};


// end of file
