#include "WrappedJsonValue.h"

#include <cassert>
#include <stdexcept>

Json::Value WrappedJsonValue::m_nullJson;
WrappedJsonValue WrappedJsonValue::m_null;

WrappedJsonValue::WrappedJsonValue()
    : m_val(m_nullJson)
    , m_memberIndex(-1)
{
    m_impl.childrenByIndex = nullptr;
    m_impl.childrenByName = nullptr;
}

WrappedJsonValue::WrappedJsonValue(const std::string& memberName, Json::Value& value)
    : m_val(value)
    , m_memberName(memberName)
    , m_memberIndex(-1)
{
    m_impl.childrenByIndex = nullptr;
    m_impl.childrenByName = nullptr;
    
    if (m_val.isObject())
    {
        m_impl.childrenByName = new std::map<std::string, WrappedJsonValue>();
        ExpandFrom(m_val);
    }
    else if (m_val.isArray())
    {
        m_impl.childrenByIndex = new std::map<int, WrappedJsonValue>();
        ExpandFrom(m_val);
    }
}

WrappedJsonValue::WrappedJsonValue(int memberIndex, Json::Value& value)
    : m_val(value)
    , m_memberIndex(memberIndex)
{
    m_impl.childrenByIndex = nullptr;
    m_impl.childrenByName = nullptr;
    
    if (m_val.isObject())
    {
        m_impl.childrenByName = new std::map<std::string, WrappedJsonValue>();
        ExpandFrom(m_val);
    }
    else if (m_val.isArray())
    {
        m_impl.childrenByIndex = new std::map<int, WrappedJsonValue>();
        ExpandFrom(m_val);
    }
}

WrappedJsonValue::WrappedJsonValue(const WrappedJsonValue& o)
    :m_val(o.m_val)
    ,m_memberName(o.m_memberName)
    ,m_memberIndex(o.m_memberIndex)
{
    if (m_val.isObject())
    {
        m_impl.childrenByName = new std::map<std::string, WrappedJsonValue>();
        
        for(auto it = o.m_impl.childrenByName->begin(); it != o.m_impl.childrenByName->end(); ++it)
            m_impl.childrenByName->operator[](it->first) = it->second;
    }
    else if (m_val.isArray())
    {
        m_impl.childrenByIndex = new std::map<int, WrappedJsonValue>();
        
        for(auto it = o.m_impl.childrenByIndex->begin(); it != o.m_impl.childrenByIndex->end(); ++it)
            m_impl.childrenByIndex->operator[](it->first) = it->second;
    }
}

WrappedJsonValue::WrappedJsonValue(WrappedJsonValue&& o) noexcept
    : m_val(o.m_val)
    , eventChange(std::move(o.eventChange))
    , m_memberName(std::move(o.m_memberName))
    , m_memberIndex(o.m_memberIndex)
    , m_impl(o.m_impl)
{
    o.m_impl.childrenByIndex = nullptr;
    o.m_impl.childrenByName = nullptr;
}

WrappedJsonValue::~WrappedJsonValue()
{
    if (m_val.isObject() && m_impl.childrenByName)
    {
        delete m_impl.childrenByName;
        m_impl.childrenByName = nullptr;
    }
    else if (m_val.isArray() && m_impl.childrenByIndex)
    {
        delete m_impl.childrenByIndex;
        m_impl.childrenByIndex = nullptr;
    }
}

const WrappedJsonValue& WrappedJsonValue::operator[](const std::string &path) const
{
    if (m_val.isNull() || !m_val.isObject())
        return m_null;
    
    auto it = m_impl.childrenByName->find(path);
    if (it != m_impl.childrenByName->end())
        return it->second;
    
    return m_null;
}

WrappedJsonValue& WrappedJsonValue::operator[](const std::string &path)
{
    return const_cast<WrappedJsonValue&>(const_cast<const WrappedJsonValue*>(this)->operator[](path));
}

const WrappedJsonValue& WrappedJsonValue::GetValueByPath(const std::string& path) const
{
    const WrappedJsonValue* val = this;
    std::vector<std::string> elems = split(path);
    for(int i1(0); i1 < elems.size(); i1++)
    {
        if (val->m_val.isObject())
        {
            val = &val->operator[](elems[i1]);
            if(val->m_val.isNull())
                return *val;
        }
        else
            return m_null;
    }
    return *val;
}

WrappedJsonValue& WrappedJsonValue::GetValueByPath(const std::string& path)
{
    return const_cast<WrappedJsonValue&>(const_cast<const WrappedJsonValue*>(this)->GetValueByPath(path));
}

std::vector<std::string> WrappedJsonValue::split(const std::string& ss) const
{
    std::vector<std::string> res;

    std::string s = ss;
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        res.emplace_back(token);
        s.erase(0, pos + delimiter.length());
    }
    res.emplace_back(s);

    return res;
}

void WrappedJsonValue::ExpandFrom(Json::Value& value)
{
    if (value.isObject())
    {
        auto members = value.getMemberNames();
        
        for (auto it : members)
        {
            Json::Value& v = value[it];
            m_impl.childrenByName->insert({it, WrappedJsonValue(it, v)});
        }
    }
    else if (value.isArray())
    {
        for (int i = 0; i < value.size(); ++i)
        {
            Json::Value& v = value[i];
            m_impl.childrenByIndex->insert({i, WrappedJsonValue(i, v)});
        }
    }
}

void WrappedJsonValue::InvokeOnValueChanged()
{
    if (eventChange)
        eventChange(m_val);
}


template<>
bool WrappedJsonValue::Get() const
{
    assert(m_val.type() == Json::ValueType::booleanValue);
    return m_val.asBool();
}

template<> int WrappedJsonValue::Get() const
{
    assert(m_val.type() == Json::ValueType::intValue);
    return m_val.asInt();
}

template<> float WrappedJsonValue::Get() const
{
    assert(m_val.type() == Json::ValueType::realValue);
    return m_val.asDouble();
}

template<> double WrappedJsonValue::Get() const
{
    assert(m_val.type() == Json::ValueType::realValue);
    return m_val.asDouble();
}

template<> std::string WrappedJsonValue::Get() const
{
    assert(m_val.type() == Json::ValueType::stringValue);
    return m_val.asString();
}

template<>
const std::map<int, WrappedJsonValue>& WrappedJsonValue::Get() const
{
    assert(m_val.type() == Json::ValueType::arrayValue);
    assert(m_impl.childrenByIndex);
    return *m_impl.childrenByIndex;
}

bool WrappedJsonValue::Remove(const std::string& name)
{
    assert(!m_val.isNull());
    assert(m_val.type() == Json::ValueType::objectValue);

    if (!m_val.isMember(name))
        return false;
    
    m_val.removeMember(name);
    
    auto it = m_impl.childrenByName->find(name);
    assert(it != m_impl.childrenByName->end());
    m_impl.childrenByName->erase(it);
    
    InvokeOnValueChanged();
    return true;
}

WrappedJsonValue& WrappedJsonValue::operator[](int index)
{
    assert(m_val.isNull() || m_val.isArray());
    
    Json::Value& v = m_val.operator[](index);
    
    int key( index );
    auto it = m_impl.childrenByIndex->lower_bound( key );
    if ( it != m_impl.childrenByIndex->end()  &&  (*it).first == key )
        return (*it).second;

    it = m_impl.childrenByIndex->insert( it, {index, WrappedJsonValue(index, v)} );
    return (*it).second;
}

WrappedJsonValue& WrappedJsonValue::operator=(const WrappedJsonValue& other)
{
    if (this == &other)
        return *this;
    
    bool changed = m_val != other.m_val;
    
    WrappedJsonValue temp(other);
    swap(temp);
    
    m_val = other.m_val;
    
    if (changed)
        InvokeOnValueChanged();
    
    return *this;
}

WrappedJsonValue& WrappedJsonValue::operator=(WrappedJsonValue&& other) noexcept
{
    m_memberName = std::move(other.m_memberName);
    m_memberIndex = std::move(other.m_memberIndex);
    
    m_impl.childrenByName = std::move(other.m_impl.childrenByName);
    m_impl.childrenByIndex = std::move(other.m_impl.childrenByIndex);
    
    other.m_impl.childrenByName = nullptr;
    other.m_impl.childrenByIndex = nullptr;
    
    eventChange = std::move(other.eventChange);
    other.eventChange = nullptr;
    
    m_val = std::move(other.m_val);
    other.m_val = Json::nullValue;
    
    InvokeOnValueChanged();
    
    return *this;
}

void WrappedJsonValue::swap( WrappedJsonValue& other )
{
    std::swap(m_memberIndex, other.m_memberIndex);
    std::swap(m_memberName, other.m_memberName);
    std::swap(m_impl, other.m_impl);
}

bool WrappedJsonValue::Remove(int index)
{
    assert(!m_val.isNull());
    assert(m_val.type() == Json::ValueType::arrayValue);
    assert(m_val.isValidIndex(index));
    
    int key(index);
    auto it = m_impl.childrenByIndex->find(key);
    if (it == m_impl.childrenByIndex->end())
        return false;
    
    int oldSize = m_impl.childrenByIndex->size();
    for (int i = index; i < (oldSize - 1); ++i) {
        int keey(i);
        (*m_impl.childrenByIndex)[keey].swap((*this)[i + 1]);
    }
        
    // erase the last one ("leftover")
    int keyLast(oldSize - 1);
    auto itLast = m_impl.childrenByIndex->find(keyLast);
    m_impl.childrenByIndex->erase(itLast);
    
    Json::Value removed;
    assert(m_val.removeIndex(index, &removed));
    InvokeOnValueChanged();
    return true;
}
