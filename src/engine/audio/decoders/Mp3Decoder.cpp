#include "Mp3Decoder.hpp"
#include "mp3reader.h" // external

CMp3Decoder::CMp3Decoder(const std::string& fileName)
 : CAudioDecoder(fileName)
{
}

bool CMp3Decoder::decode()
{
    if (m_data == nullptr)
        return false;

    mp3_callbacks callbacks;
    callbacks.read = CAudioDecoder::fileRead;
    callbacks.seek = CAudioDecoder::fileSeek;
    callbacks.close = CAudioDecoder::fileClose;
    callbacks.tell = CAudioDecoder::fileTell;
        
    if (EXIT_SUCCESS == decodeMP3(&callbacks, this, m_buffer, &m_channels, &m_bitsPerSample, &m_numFrames)
        && m_channels > 0 && m_bitsPerSample > 0 && m_numFrames > 0)
    {
        m_duration = 1.0f * m_numFrames / m_bitsPerSample;
        return true;
    }
    
    return false;
}
