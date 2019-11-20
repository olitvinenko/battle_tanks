#pragma once
 
#include "WrappedJsonValue.h"

class JsonConfig
{
private:
    JsonConfig();
public:
    operator WrappedJsonValue&() { return m_root; }
    
    WrappedJsonValue* operator->() { return &m_root; }
    const WrappedJsonValue* operator->() const { return &m_root; }
    
    static std::shared_ptr<JsonConfig> MakeFrom(const std::string& rawConfig);
private:
    WrappedJsonValue m_root;
};
