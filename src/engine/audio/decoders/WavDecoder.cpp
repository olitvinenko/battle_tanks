#include "WavDecoder.hpp"
#include "tinysndfile.h" // external

CWavDecoder::CWavDecoder(const std::string& fileName)
    :CAudioDecoder(fileName)
{
}

void* CWavDecoder::onWavOpen(const char* path, void* user)
{
    return user;
}

int CWavDecoder::onWavSeek(void* datasource, long offset, int whence)
{
    return CAudioDecoder::fileSeek(datasource, (int64_t) offset, whence);
}

int CWavDecoder::onWavClose(void* datasource)
{
    return 0;
}

bool CWavDecoder::decode()
{
    if (m_data == nullptr)
        return false;
    
    SF_INFO info;
    
    snd_callbacks cb;
    cb.open = onWavOpen;
    cb.read = CAudioDecoder::fileRead;
    cb.seek = onWavSeek;
    cb.close = onWavClose;
    cb.tell = CAudioDecoder::fileTell;
    
    SNDFILE* handle = NULL;
    bool ret = false;
    do
    {
        handle = sf_open_read(m_fileName.c_str(), &info, &cb, this);
        if (handle == nullptr)
            break;
        
        if (info.frames == 0)
            break;
        
        //LOG("wav info: frames: %d, samplerate: %d, channels: %d, format: %d", info.frames, info.samplerate, info.channels, info.format);
        size_t bufSize = sizeof(short) * info.frames * info.channels;
        unsigned char* buf = (unsigned char*)malloc(bufSize);
        sf_readf_short(handle, (short*)buf, info.frames);
        
        m_buffer.insert(m_buffer.end(), buf, buf + bufSize);
        m_channels = info.channels;
        m_bitsPerSample = info.samplerate;
        m_numFrames = info.frames;
        m_duration = 1.0f * m_numFrames / m_bitsPerSample;
        
        free(buf);
        ret = true;
    } while (false);
    
    if (handle != NULL)
        sf_close(handle);
    
    return ret;
}
