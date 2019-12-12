#pragma once

#include <cstddef>
#include <cstdlib>

#include <string>
#include <vector>

class CAudioDecoder
{
public:
    static CAudioDecoder* CreateAudioDecoder(const std::string& fileName);
    
    CAudioDecoder(const std::string& fileName);
    virtual ~CAudioDecoder();
    virtual bool decode() = 0;
    
    float GetDuration(){return m_duration;}
    int GetChannels(){return m_channels;}
    int GetSampleRate(){return m_bitsPerSample;}
    int GetBitsPerSample(){return 16;}
    int GetNumFrames(){return m_numFrames;}
    std::vector<char>& GetBuffer() {return m_buffer;}

protected:
    static size_t fileRead(void* ptr, size_t size, size_t nmemb, void* datasource);
    static int fileSeek(void* datasource, int64_t offset, int whence);
    static int fileClose(void* datasource);
    static long fileTell(void* datasource);
    
private:
    CAudioDecoder(const CAudioDecoder&) = delete;
    CAudioDecoder(CAudioDecoder&&) noexcept = delete;
    CAudioDecoder& operator=(const CAudioDecoder&) = delete;
    CAudioDecoder& operator=(CAudioDecoder&&) noexcept = delete;

protected:
    void*   m_data = nullptr;
    size_t  m_size = 0;
    
    size_t  m_fileCurrPos = 0;
    std::string m_fileName;
    std::vector<char> m_buffer;
    float   m_duration = 0;
    int     m_channels = 0;
    int     m_bitsPerSample = 0;
    int     m_numFrames = 0;
};
