#pragma once

#include <cstdint>
#include <string>
#include <map>

#include <OpenAL/OpenAL.h>

class OalSound;
class SoundEngine;

class OalBuffer : public std::enable_shared_from_this<OalBuffer>
{
    friend class SoundEngine;

public:
    OalBuffer(const std::string& fileName, SoundEngine* engine);
    ~OalBuffer();
    
    ALdouble Duration() const { return m_duration; }
    const std::string& GetFileName() const { return m_fileName; }
    
    bool LoadMemory();
    bool CanBeErased() const;
    void UnloadMem();
    
    float SizeMem() const { return m_sizeMemory; }
    
    ALuint GetSource(OalSound* soundOAL);
    bool RemoveSource(OalSound* soundOAL);

private:
    std::map<OalSound*, ALuint>  m_mapSources;
    int m_sourcesCount = 0;
    
    ALuint      m_bufferID = 0;
    ALdouble    m_duration = 0;
    std::string m_fileName;
    float       m_sizeMemory = .0f;
    
    SoundEngine* m_soundEngine;
};
