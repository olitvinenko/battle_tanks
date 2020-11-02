#pragma once

#include <OpenAL/OpenAL.h>

#include <unordered_map>
#include <string>
#include <memory>
#include <list>

class OalBuffer;
class OalSound;
class SoundHandle;

class SoundEngine
{
public:
    SoundEngine();
    ~SoundEngine();
    
    float GetMaxMem() const { return m_maxMem; };
    float GetCurMem() const { return m_curMem; };
    
    void Update(float dt);
    
    bool ActivateBuffer(std::shared_ptr<OalBuffer> buffer);
    bool DeactivateBuffer(std::shared_ptr<OalBuffer> buffer);
    void IncrementMemory(float sizeMem);
    void DecrementMemory(float sizeMem);
    
    bool PlayOnce(const std::string& fileName);
    SoundHandle Play(const std::string& fileName, bool isAutoDelete);
    SoundHandle GetSound(const std::string& fileName, bool isAutoDelete);
    
private:
    void AddSound(std::shared_ptr<OalSound> sound);
    std::shared_ptr<OalSound> CreateSound(const std::string& fileName, bool isAutoDelete);
    
    std::unordered_map<std::string, std::shared_ptr<OalBuffer>> m_buffers;
    
    using SoundList = std::list<std::shared_ptr<OalSound>>;
    std::unordered_map<std::string, SoundList> m_sounds;
    
    ALCdevice* m_device;
    ALCcontext* m_context;

    bool m_initialized;
    
    float m_maxMem;
    float m_curMem;
};
