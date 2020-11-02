#include "OalBuffer.hpp"

#include "OalSound.hpp"
#include "SoundEngine.hpp"

#include "AudioDecoder.hpp"

#include <fstream>
#include <iostream>

static ALint GetFormatSound(ALint channels, ALint bitsPerSample)
{
    ALint format=AL_FORMAT_MONO8;
    if ( channels == 2 ) // stereo
    {
        if ( bitsPerSample == 16 )
            format = AL_FORMAT_STEREO16;
        else
            format = AL_FORMAT_STEREO8;
    }
    else // mono
    {
        if ( bitsPerSample == 16 )
            format = AL_FORMAT_MONO16;
        else
            format = AL_FORMAT_MONO8;
    }
    return format;
}

OalBuffer::OalBuffer(const std::string& fileName, SoundEngine* engine)
    : m_fileName(fileName)
    , m_soundEngine(engine)
{
}

bool OalBuffer::LoadMemory()
{
    CAudioDecoder* decoder = CAudioDecoder::CreateAudioDecoder(m_fileName);
    if(!decoder || !decoder->decode())
        return false;
    
    ALvoid* bufferData = &decoder->GetBuffer()[0];
    ALint format = GetFormatSound(decoder->GetChannels(), decoder->GetBitsPerSample());
    ALsizei size = (ALsizei)decoder->GetBuffer().size();
    ALsizei freq = decoder->GetSampleRate();
    int duration = decoder->GetDuration();
    
    alGenBuffers(1, &m_bufferID);
    alBufferData(m_bufferID, format, bufferData, size, freq);
    
    if (alGetError() != AL_NO_ERROR)
    {
        if (m_bufferID && alIsBuffer(m_bufferID))
            alDeleteBuffers(1, &m_bufferID);
        
        delete decoder;
        
        m_bufferID = 0;
        return false;
    }
    
    m_duration = duration;
    m_sizeMemory = (float)size / (1024.0f * 1024.0f);
    m_soundEngine->IncrementMemory(SizeMem());
    
    delete decoder;
    
    return true;
}

OalBuffer::~OalBuffer()
{
    if (m_bufferID && alIsBuffer(m_bufferID))
        alDeleteBuffers(1, &m_bufferID);
    
    m_bufferID = 0;
}

bool OalBuffer::CanBeErased() const
{
    if (!m_mapSources.empty())
        return false;
    
    if (m_bufferID == 0) // already unloaded
        return false;
    
    return true;
}

void OalBuffer::UnloadMem()
{
    auto it = m_mapSources.begin();
    for (; it != m_mapSources.end() ; it++)
    {
        it->first->UnloadBuffer();
        ALuint sourceID = it->second;
        alSourceStop(sourceID);
        alSourcei(sourceID, AL_BUFFER, 0);
        alDeleteSources(1, &sourceID);

        m_sourcesCount--;
    }
    m_mapSources.clear();
    
    //now the buffer, since it wasn't copied to openAL
    if (m_bufferID)
    {
        if (alIsBuffer(m_bufferID))
            alDeleteBuffers(1, &m_bufferID);
//        else
//            throw ("Error: alIsBuffer(m_bufferID)");
    }
    
    m_bufferID = 0;
}
#define SOURCE_OAL 32 //макс допустимое количество источников
ALuint OalBuffer::GetSource(OalSound* soundOAL)
{
    ALuint sourceID(0);

    auto it = m_mapSources.find(soundOAL);
    if (m_mapSources.end() != it)
    {
        return it->second;
    }
    
    const static int MAX_SOURCE = 8; //TODO::
    if (m_sourcesCount >= MAX_SOURCE)
    {
        std::cout << "m_CounterSource >= MAX_SOURCE" << std::endl;
        //throw ("m_CounterSource >= MAX_SOURCE");
        return 0;
    }
    
    m_soundEngine->ActivateBuffer(shared_from_this());
    
    if (m_bufferID == 0)
        return 0;
    
    ALenum error = AL_NO_ERROR;
    error = alGetError();
    
    alGenSources(1, &sourceID);
    error = alGetError();
    //TODO:: error handling
    alSourcei(sourceID, AL_BUFFER, m_bufferID);
    error = alGetError();

    if ((error = alGetError()) != AL_NO_ERROR || sourceID == 0)
    {
        //throw ("error: %i create sourceID: %i\n", error, sourceID");
        return 0;
    }
    else
    {
        m_sourcesCount++;
        m_mapSources[soundOAL] = sourceID;
    }
    
    return sourceID;
}

bool OalBuffer::RemoveSource(OalSound* soundOAL)
{
    auto it = m_mapSources.find(soundOAL);
    if (m_mapSources.end() == it)
    {
        return false;
    }
    
    //delete base source...
    ALenum error = AL_NO_ERROR;
    alGetError();
    ALuint sourceID = it->second;
    alSourceStop(sourceID);
    alSourcei(sourceID, AL_BUFFER, 0);
    alDeleteSources(1, &sourceID);
    if((error = alGetError()) != AL_NO_ERROR || sourceID==0)
    {
        //throw ("error: %i Delete sourceID: %i\n", error, sourceID);
    }
    

    m_sourcesCount--;
    m_mapSources.erase(it);
    
    m_soundEngine->DeactivateBuffer(shared_from_this());
    
    return true;
}

