
#include "JsonConfig.h"

JsonConfig::JsonConfig()
{
}

std::shared_ptr<JsonConfig> JsonConfig::MakeFrom(const std::string& rawConfig)
{
    Json::Value value;
    Json::Reader reader;
    if (!reader.parse(rawConfig, value))
        return nullptr;
    
    std::shared_ptr<JsonConfig> res(new JsonConfig);
    res->m_root = WrappedJsonValue("root", value);
    return res;
}
