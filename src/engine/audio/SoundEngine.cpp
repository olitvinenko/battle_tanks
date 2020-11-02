
#include "SoundEngine.hpp"

#include "OalSound.hpp"
#include "OalBuffer.hpp"
#include "SoundHandle.hpp"

#include <algorithm>

SoundEngine::SoundEngine()
    : m_device(nullptr)
    , m_context(nullptr)
    , m_initialized(false)
    , m_maxMem(15.f)
    , m_curMem (0.f)
{
    m_device = alcOpenDevice(NULL);
    if (!m_device)
        return;
       
    assert(alGetError() == AL_NO_ERROR);
    
    m_context = alcCreateContext(m_device, NULL);
    if (!m_context)
    {
          alcCloseDevice(m_device);
          return;
    }
    
    assert(alGetError() == AL_NO_ERROR);

    alcMakeContextCurrent(m_context);
    m_initialized = true;
}

SoundEngine::~SoundEngine()
{
    if (!m_initialized)
        return;
       
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void SoundEngine::IncrementMemory(float sizeMem)
{
    m_curMem += sizeMem;
}

void SoundEngine::DecrementMemory(float sizeMem)
{
    m_curMem -= sizeMem;
    assert(m_curMem >= 0);
}

void SoundEngine::Update(float dt)
{
    auto it = m_sounds.begin();
    while (it != m_sounds.end())
    {
        auto jt = it->second.begin();
        while (jt != it->second.end())
        {
            auto sound = *jt;
            if (sound->m_isAutoDelete && sound->IsStop())
            {
                jt = it->second.erase(jt);
            }
            else if (sound->m_sourceID != 0 && !sound->IsLoop() && sound->IsStop())
            {
                sound->m_buffer->RemoveSource(sound.get());
                sound->m_sourceID = 0;
                sound->m_currentTime = 0;
                
                ++jt;
            }
            else if (sound->m_isAutoDelete && sound->m_sourceID == 0 && !sound->IsLoop())
            {
                // first = inside m_sounds map
                // second - local variable inside while loop
                static const size_t UNUSED_SOUND_COUNT = 2;
                if (sound.use_count() == UNUSED_SOUND_COUNT)
                    jt = it->second.erase(jt);
            }
            else
            {
                ++jt;
            }
        }
        
        if (it->second.empty())
            it = m_sounds.erase(it);
        else
            it++;
    }
}

bool SoundEngine::PlayOnce(const std::string& fileName)
{
    std::shared_ptr<OalSound> sound = CreateSound(fileName, true);
    return sound->Play();
}

SoundHandle SoundEngine::Play(const std::string& fileName, bool isAutoDelete)
{
    std::shared_ptr<OalSound> sound = CreateSound(fileName, isAutoDelete);
    assert(sound->Play());
    return SoundHandle(sound);
}

SoundHandle SoundEngine::GetSound(const std::string& fileName, bool isAutoDelete)
{
    std::shared_ptr<OalSound> sound = CreateSound(fileName, isAutoDelete);
    return SoundHandle(sound);
}

std::shared_ptr<OalSound> SoundEngine::CreateSound(const std::string& fileName, bool isAutoDelete)
{
    auto it = m_buffers.find(fileName);
    if (it != m_buffers.end())
    {
        auto sound = std::make_shared<OalSound>(it->second, isAutoDelete, this);
        AddSound(sound);
        return sound;
    }
    else
    {
        auto buffer = std::make_shared<OalBuffer>(fileName, this);
        m_buffers[fileName] = buffer;
        
        auto sound = std::make_shared<OalSound>(buffer, isAutoDelete, this);
        AddSound(sound);
        return sound;
    }
}

void SoundEngine::AddSound(std::shared_ptr<OalSound> sound)
{
    auto it = m_sounds.find(sound->GetFileName());
    if (it != m_sounds.end())
    {
        it->second.push_back(sound);
    }
    else
    {
        auto res = m_sounds.insert({sound->GetFileName(), SoundList()});
        assert(res.second);
        res.first->second.push_back(sound);
    }
}

bool SoundEngine::DeactivateBuffer(std::shared_ptr<OalBuffer> buffer)
{
    if (!buffer->CanBeErased())
        return false;

    buffer->UnloadMem();
    DecrementMemory(buffer->m_sizeMemory);
    
    return true;
}

bool SoundEngine::ActivateBuffer(std::shared_ptr<OalBuffer> buffer)
{
    if (buffer->m_bufferID)
        return true;
    
    if (!buffer->LoadMemory())
        return false;
    
    if (buffer->SizeMem() >= GetMaxMem())
    {
        buffer->UnloadMem();
        
#ifndef NDEBUG
        assert(false);
#endif
        return false;
    }
    
    if (GetCurMem() >= GetMaxMem())
    {
        for (auto it = m_buffers.begin(); it != m_buffers.end(); ++it)
        {
            auto buffer = it->second;
            std::map<OalSound*, ALuint>& sources(buffer->m_mapSources);
            
            bool allSourcesAreNotPlaying = std::all_of(sources.begin(), sources.end(), [](std::pair<OalSound*, ALuint> it) {
                return !it.first->IsPlaying();
            });
            
            if (!allSourcesAreNotPlaying)
                continue;
            
            buffer->UnloadMem();
            
            if (GetCurMem() < GetMaxMem())
                break;
        }
        
#ifndef NDEBUG
        assert(false);
#endif
        return false;
    }
    
    return true;
}
