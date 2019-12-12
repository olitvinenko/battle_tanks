#pragma once

#include <OpenAL/OpenAL.h>
#include <memory>

class SoundEngine;
class OalBuffer;

class OalSound
{
    friend class SoundEngine;
    
public:
    OalSound(std::shared_ptr<OalBuffer> buffer, bool isAutoDelete, SoundEngine* engine);
    ~OalSound();
    
    OalSound* SetVolume(float volume);
    
    bool Play();
    bool IsPlaying() const;
    
    bool Stop();
    bool IsStop() const;
    
    bool Loop(bool loop);
    bool IsLoop();
    
    bool Pause();
    
    float GetCurrentTime();
    bool SetCurrentTime(float currentTime);

    void UnloadBuffer();
    
    const std::string& GetFileName() const;
    
private:
    bool Volume(float volume);
    
private:
    ALfloat mCurrentTime;
    ALfloat m_saveCurrentTime;
    
    bool m_isLoop;
    float mVolume;
    
    float   mDuration;
    bool m_isAutoDelete;
    
    ALuint m_sourceID;
    
    std::shared_ptr<OalBuffer> m_buffer;
    SoundEngine* m_engine;
};
