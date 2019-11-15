
#include "JsonConfig.h"

JsonConfig::JsonConfig(const std::string& rawConfig)
{
    Json::Value value;
    Json::Reader reader;
    if (!reader.parse(rawConfig, value))
        return;
    
    m_root = WrappedJsonValue("root", value);
}

std::shared_ptr<JsonConfig> JsonConfig::MakeFrom(const std::string& rawConfig)
{
    return std::make_shared<JsonConfig>(rawConfig);
}
