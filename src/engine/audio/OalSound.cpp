#include "OalSound.hpp"
#include "OalBuffer.hpp"

OalSound::OalSound(std::shared_ptr<OalBuffer> buffer, bool isAutoDelete, SoundEngine* engine)
        : m_buffer(buffer)
        , m_engine(engine)
        , m_sourceID(0)
        , m_currentTime(0)
        , m_saveCurrentTime(0)
        , m_isLoop(false)
        , m_volume(1)
        , m_isAutoDelete(isAutoDelete)
{
}
    
OalSound::~OalSound()
{
    Stop();
}

const std::string& OalSound::GetFileName() const
{
    static const std::string empty;
    if (!m_buffer)
        return empty;
    
    return m_buffer->GetFileName();
}

void OalSound::UnloadBuffer()
{
    if (m_sourceID)
    {
        //alGetSourcei(m_sourceID, AL_SOURCE_STATE, &m_saveState);
        //alGetSourcef(m_sourceID, AL_SEC_OFFSET, &m_saveCurrentTime);
    }
    m_sourceID = 0;
    m_currentTime = 0;
}


OalSound* OalSound::SetVolume(float volume)
{
    m_volume = std::max(std::min(volume, 1.0f), 0.0f);
    Volume(m_volume);
    return this;
}
    
bool OalSound::Play()
{
    if (IsPlaying())
        return true;

    if (m_buffer && !m_sourceID)
    {
        m_sourceID = m_buffer->GetSource(this);
        Loop(m_isLoop);
        
        m_volume = 100;
        SetVolume(m_volume);
    }

    if (m_sourceID)
    {
        alGetError();
        alSourcePlay(m_sourceID);
        return alGetError() == AL_NO_ERROR;
    }

    return false;
}

bool OalSound::Stop()
{
    if (m_sourceID)
    {
        m_saveCurrentTime = 0;
        
        alGetError();
        alSourceStop(m_sourceID);
        if(alGetError() != AL_NO_ERROR)
        {
            //throw("error stop file:%s\n", m_file.data());
            return false;
        }
        if (m_buffer)
        {
            m_buffer->RemoveSource(this);
            m_sourceID = 0;
        }
        return true;
    }
    return false;
}
    
bool OalSound::IsPlaying() const
{
    if (m_sourceID)
    {
        ALenum state(-1);
        alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
        
        return state == AL_PLAYING;
    }
    return false;
}
    
bool OalSound::IsStop() const
{
    if (m_sourceID)
    {
        ALenum state(-1);
        alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
        
        return state == AL_STOPPED;
    }
    return false;
}
    
bool OalSound::Pause()
{
    if (m_sourceID)
    {
        m_saveCurrentTime = GetCurrentTime();
        
        alGetError();
        alSourcePause(m_sourceID);
        if(alGetError() != AL_NO_ERROR)
        {
            //throw ("error pause file:%s\n", m_file.data());
            return false;
        }
        
        return true;
    }
    return false;
}

float OalSound::GetCurrentTime ()
{
    if (m_sourceID)
        alGetSourcef(m_sourceID, AL_SEC_OFFSET, &m_currentTime);
    else
        m_currentTime = 0;
    
    return m_currentTime;
}
    
bool OalSound::SetCurrentTime(float currentTime)
{
    if (m_sourceID)
    {
        if (currentTime < 0) currentTime=0;
        else if (currentTime > m_duration) currentTime = m_duration;
        m_currentTime = currentTime;
        
        alGetError();
        alSourcef(m_sourceID, AL_SEC_OFFSET, m_currentTime);
        if(alGetError() != AL_NO_ERROR)
        {
            //throw ("error SetCurrentTime file:%s\n", m_file.data());
            return false;
        }
        return true;
    }
    return false;
}

bool OalSound::Loop(bool loop)
{
    m_isLoop = loop;

    if (m_sourceID)
    {
        alSourcei(m_sourceID, AL_LOOPING, m_isLoop ? AL_TRUE : AL_FALSE);
        return true;
    }
    return false;
}

bool OalSound::IsLoop()
{
    if (m_sourceID)
    {
        ALint state(-1);
        alGetSourcei(m_sourceID, AL_LOOPING, &state);
        return  m_isLoop = state == AL_TRUE;
    }
    return m_isLoop;
}

bool OalSound::Volume(float volume)
{
    if (m_sourceID)
    {
        ALfloat vol = std::max(std::min(volume, 1.0f), 0.0f);
        
        alGetError();
        alSourcef(m_sourceID, AL_GAIN, vol);
        if(alGetError() != AL_NO_ERROR)
        {
            //throw ("error SetVolume file:%s\n", m_file.data());
            return false;
        }
        return true;
    }
    return false;
}
