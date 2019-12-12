#include "AudioDecoder.hpp"
#include "Mp3Decoder.hpp"
#include "WavDecoder.hpp"

#include <fstream>

size_t CAudioDecoder::fileRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    CAudioDecoder* thiz = (CAudioDecoder*)datasource;
    ssize_t toReadBytes = std::min((ssize_t)(thiz->m_size - thiz->m_fileCurrPos), (ssize_t)(nmemb * size));
    if (toReadBytes > 0)
    {
        memcpy(ptr, (unsigned char*) thiz->m_data + thiz->m_fileCurrPos, toReadBytes);
        thiz->m_fileCurrPos += toReadBytes;
    }
    // ALOGD("File size: %d, After fileRead _fileCurrPos %d", (int)thiz->_fileData.getSize(), thiz->_fileCurrPos);
    return toReadBytes;
}

int CAudioDecoder::fileSeek(void* datasource, int64_t offset, int whence)
{
    CAudioDecoder* thiz = (CAudioDecoder*)datasource;
    if (whence == SEEK_SET)
        thiz->m_fileCurrPos = offset;
    else if (whence == SEEK_CUR)
        thiz->m_fileCurrPos = thiz->m_fileCurrPos + offset;
    else if (whence == SEEK_END)
        thiz->m_fileCurrPos = thiz->m_size;
    return 0;
}

int CAudioDecoder::fileClose(void* datasource)
{
    return 0;
}

long CAudioDecoder::fileTell(void* datasource)
{
    CAudioDecoder* thiz = (CAudioDecoder*)datasource;
    return (long) thiz->m_fileCurrPos;
}

CAudioDecoder* CAudioDecoder::CreateAudioDecoder(const std::string& fileName)
{
    std::string path(fileName);
    
    std::string::size_type idx;
    idx = path.rfind('.');
    if(idx == std::string::npos)
        return nullptr;
    
    std::string extension = path.substr(idx + 1);
    
    if(extension == "mp3")
        return new CMp3Decoder(fileName);
    else if(extension == "wav")
        return new CWavDecoder(fileName);
    
    return nullptr;
}

CAudioDecoder::CAudioDecoder(const std::string& fileName)
    : m_fileName(fileName)
{
    std::fstream fs(fileName);
    if (!fs.is_open())
        return;
    
    fs.seekg(0, std::ios::end);
    
    m_size = fs.tellg();
    m_data = malloc(m_size);
    if (!m_data)
    {
        fs.close();
        return; // throw std::bad_alloc();
    }
    
    fs.seekg(0, std::ios::beg);
    fs.read((char*)m_data, m_size);
    
    fs.close();
}

CAudioDecoder::~CAudioDecoder()
{
    if (m_data)
        free(m_data);
}
