#include "SoundHandle.hpp"

#include "OalSound.hpp"

SoundHandle::SoundHandle(std::weak_ptr<OalSound> sound)
   : m_sound(sound)
{
}

OalSound* SoundHandle::operator ->()
{
    OalSound* ptr = *this;
    assert(ptr);
    return ptr;
}

SoundHandle::operator OalSound*()
{
    if (auto ptr = m_sound.lock())
        return ptr.get();
        
    return nullptr;
}

SoundHandle::~SoundHandle()
{
    OalSound* ptr = *this;
    if (ptr && ptr->IsPlaying())
        assert(ptr->Stop());
}
