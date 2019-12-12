#include <algorithm>
#include <cstddef>

#include "TGAImage.h"

#include "filesystem/FileSystem.h"

TgaImage::TgaImage(const void *data, unsigned long size)
{
    static const unsigned char signatureU[12] = { 0,0,2, 0,0,0,0,0,0,0,0,0 }; // Uncompressed
    static const unsigned char signatureC[12] = { 0,0,10,0,0,0,0,0,0,0,0,0 }; // Compressed

    struct Header
    {
        unsigned char signature[12];
        unsigned char header[6]; // First 6 useful bytes from the header
        unsigned char data[1];
    };

    if (size < sizeof(Header))
    {
        throw std::runtime_error("corrupted TGA image");
    }

    const Header &h = *(const Header *)data;
    unsigned long dataSize = size - offsetof(Header, data);

    m_width = h.header[1] * 256 + h.header[0];
    m_height = h.header[3] * 256 + h.header[2];
    m_bpp = h.header[4];

    if (m_width <= 0 || m_height <= 0 || (m_bpp != 24 && m_bpp != 32))
    {
        throw std::runtime_error("unsupported size or bpp");
    }

    const unsigned int bytesPerPixel = m_bpp / 8;
    const unsigned int imageSizeBytes = bytesPerPixel * m_width * m_height;
    const unsigned int pixelCount = m_height * m_width;

    if (0 == memcmp(signatureU, h.signature, 12))
    {
        if (dataSize < imageSizeBytes)
        {
            throw std::runtime_error("corrupted TGA image");
        }
        m_data.assign(h.data, h.data + imageSizeBytes);
    }
    else if (0 == memcmp(signatureC, h.signature, 12))
    {
        unsigned int currentPixel = 0;
        unsigned int currentByte = 0;
        m_data.reserve(imageSizeBytes);
        do
        {
            if (currentByte >= dataSize)
            {
                throw std::runtime_error("corrupted TGA image");
            }
            const unsigned char chunkHeader = h.data[currentByte++];

            if (chunkHeader < 128)    // If the header is < 128, it means the that is the number
            {                          // of RAW color packets minus 1 that follow the header
                                       // Read RAW color values
                int pcount = chunkHeader + 1;
                currentPixel += pcount;
                if (pixelCount < currentPixel || dataSize < currentByte + pcount * bytesPerPixel)
                {
                    throw std::runtime_error("corrupted TGA image");
                }
                m_data.insert(m_data.end(), h.data + currentByte, h.data + currentByte + pcount * bytesPerPixel);
                currentByte += pcount * bytesPerPixel;
            }
            else // chunkHeader >= 128 RLE data, next color repeated chunkHeader - 127 times
            {
                int pcount = chunkHeader - 127;  // get rid of the ID bit
                currentPixel += pcount;
                if (pixelCount < currentPixel || dataSize < currentByte + bytesPerPixel)
                {
                    throw std::runtime_error("corrupted TGA image");
                }
                const unsigned char *colorBuffer = h.data + currentByte;
                currentByte += bytesPerPixel;
                for (int counter = 0; counter < pcount; ++counter)
                {
                    m_data.insert(m_data.end(), colorBuffer, colorBuffer + bytesPerPixel);
                }
            }
        } while (currentPixel < pixelCount);
    }
    else
    {
        throw std::runtime_error("unsupported TGA signature");
    }

    // swap R <-> G
    for (unsigned int cswap = 0; cswap < imageSizeBytes; cswap += bytesPerPixel)
    {
        std::swap(m_data[cswap], m_data[cswap + 2]);
    }

    // flip vertical
    unsigned int rowSizeBytes = m_width * bytesPerPixel;
    for (unsigned long y = 0; y < m_height / 2; y++)
    {
        std::swap_ranges(m_data.begin() + y * rowSizeBytes,
            m_data.begin() + y * rowSizeBytes + rowSizeBytes,
            m_data.begin() + (m_height - y - 1) * rowSizeBytes);
    }

    // convert to 32 bit
    if (3 == bytesPerPixel)
    {
        m_data.resize(pixelCount * 4);
        for (unsigned int pixel = pixelCount; pixel--;)
        {
            *(uint32*)&m_data[pixel * 4] = (*(uint32*)&m_data[pixel * 3] & 0xffffff) | 0xff000000;
        }
        m_bpp = 32;
    }
}

TgaImage::TgaImage(std::shared_ptr<FileSystem::Memory> file)
    : TgaImage(file->GetData(), file->GetSize())
{
}
