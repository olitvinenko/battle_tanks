#pragma once

#include "AudioDecoder.hpp"

class CMp3Decoder: public CAudioDecoder
{
public:
    CMp3Decoder(const std::string& fileName);
    bool decode() override;
};
