#include "SoilImage.h"

#include "filesystem/FileSystem.h"
#include "SOIL.h"

SoilImage::SoilImage(const void *data, unsigned long size)
 : m_soilData(nullptr)
{
    int width;
    int height;
    int channels;
    m_soilData = SOIL_load_image_from_memory((const unsigned char*)data, size, &width, &height, &channels, SOIL_LOAD_AUTO);
    if (!m_soilData)
        throw std::runtime_error(SOIL_last_result());

    m_width = width;
    m_height = height;

    m_bpp = channels == SOIL_LOAD_RGBA ? 32 : 24;

    // flip vertical
//    const unsigned int bytesPerPixel = m_bpp / 8;
//    unsigned int rowSizeBytes = width * bytesPerPixel;
//    unsigned char* image = (unsigned char*)m_soilData;
//
//    for (unsigned long y = 0; y < height / 2; y++)
//    {
//        std::swap_ranges(image + y * rowSizeBytes,
//            image + y * rowSizeBytes + rowSizeBytes,
//            image + (height - y - 1) * rowSizeBytes);
//    }
}

SoilImage::SoilImage(std::shared_ptr<FileSystem::Memory> file)
    : SoilImage(file->GetData(), file->GetSize())
{
}

SoilImage::~SoilImage()
{
    if (m_soilData)
        SOIL_free_image_data(m_soilData);
}
