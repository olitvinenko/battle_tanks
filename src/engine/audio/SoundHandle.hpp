#pragma once

#include <memory>

class OalSound;

class SoundHandle
{
    friend class SoundEngine;
    
public:
    SoundHandle(SoundHandle&& handle) noexcept = default;
    SoundHandle(const SoundHandle& handle) = default;
    ~SoundHandle();
    
    OalSound* operator ->();
    operator OalSound*();
    
private:
    SoundHandle(std::weak_ptr<OalSound> sound);

private:
    std::weak_ptr<OalSound> m_sound;
};
