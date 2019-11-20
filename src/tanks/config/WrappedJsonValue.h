#pragma once
 
#include "json.h"

#include <string>
#include <functional>
#include <map>
#include <memory>

class WrappedJsonValue
{
public:
    void Print() const { std::cout << m_val.toStyledString() << std::endl; }
    
private:
    static Json::Value m_nullJson;
    static WrappedJsonValue m_null;
public:
    static WrappedJsonValue& Null() { return m_null; }
    
    WrappedJsonValue();
    WrappedJsonValue(const std::string& memberName, Json::Value& value);
    WrappedJsonValue(int memberIndex, Json::Value& value);
    WrappedJsonValue(WrappedJsonValue&& o) noexcept;
    ~WrappedJsonValue();
    
    template<typename T> T Get() const;
    
    template<> bool Get() const;
    template<> int Get() const;
    template<> float Get() const;
    template<> double Get() const;
    template<> std::string Get() const;
    
    template<> const std::map<int, WrappedJsonValue>& Get() const;
    
    void Set(bool value) { Set<bool>(std::forward<bool>(value)); }
    void Set(int&& value) { Set<int>(std::forward<int>(value)); }
    void Set(float&& value){ Set<float>(std::forward<float>(value)); }
    void Set(double&& value) { Set<double>(std::forward<double>(value)); }
    void Set(std::string&& value) { Set<std::string>(std::forward<std::string>(value)); }
    void Set(Json::Value&& value) { Set<Json::Value>(std::forward<Json::Value>(value)); }
    
    void Add(bool value) { Add<bool>(std::forward<bool>(value)); }
    void Add(int&& value) { Add<int>(std::forward<int>(value)); }
    void Add(float&& value){ Add<float>(std::forward<float>(value)); }
    void Add(double&& value) { Add<double>(std::forward<double>(value)); }
    void Add(std::string&& value) { Set<std::string>(std::forward<std::string>(value)); }
    void Add(Json::Value&& value) { Add<Json::Value>(std::forward<Json::Value>(value)); }
    
    bool Remove(const std::string& name);
    bool Remove(int index);
    
    WrappedJsonValue& GetValueByPath(const std::string& path);
    const WrappedJsonValue& GetValueByPath(const std::string& path) const;
    
    WrappedJsonValue& operator[](const std::string &path);
    const WrappedJsonValue& operator[](const std::string &path) const;
    WrappedJsonValue& operator[](int index);
    
    WrappedJsonValue& operator=(const WrappedJsonValue& other);
    WrappedJsonValue& operator=(WrappedJsonValue&& other) noexcept;
    
    WrappedJsonValue& operator=(int&& value) { return Assign<int>(std::forward<int>(value)); }
    WrappedJsonValue& operator=(float&& value) { return Assign<float>(std::forward<float>(value)); }
    WrappedJsonValue& operator=(double&& value) { return Assign<double>(std::forward<double>(value)); }
    WrappedJsonValue& operator=(bool value) { return Assign<bool>(std::forward<bool>(value)); }
    WrappedJsonValue& operator=(const char* value) { return Assign<const char*>(std::forward<const char*>(value)); }
    WrappedJsonValue& operator=(std::string&& value) { return Assign<std::string>(std::forward<std::string>(value)); }
    WrappedJsonValue& operator=(const Json::Value& value) { return Assign<const Json::Value&>(std::forward<const Json::Value&>(value)); }
    
    bool IsNull() const { return this == &m_null; }
    
    std::function<void(Json::Value&)> eventChange;
private:
    template<typename T>
    WrappedJsonValue& Assign(T&& value);
    
    WrappedJsonValue(const WrappedJsonValue& o);
    void swap( WrappedJsonValue &other );
    
    template<typename T>
    void Set(T&& value);
        
    template<typename T>
    void Add(T&& value);
    
    std::vector<std::string> split(const std::string& ss) const;
    void ExpandFrom(Json::Value& value);
    void InvokeOnValueChanged();
    
    union Data
    {
        std::map<std::string, WrappedJsonValue>*   childrenByName;
        std::map<int, WrappedJsonValue>*           childrenByIndex;
    };
private:
    Json::Value& m_val;
    std::string m_memberName;
    int         m_memberIndex;
    Data        m_impl;
};

template<typename T>
WrappedJsonValue& WrappedJsonValue::Assign(T&& value)
{
    Set<T>(std::forward<T>(value));
    return *this;
}

template<typename T>
void WrappedJsonValue::Set(T&& value)
{
    assert(!m_val.isNull());
    assert(!m_memberName.empty() || m_memberIndex != -1);
    
    Json::Value temp(value);
    bool changed = temp != m_val;
    if (!changed)
        return;
    
    m_val = value;
    
    std::string tName = m_memberName;
    int tIndex = m_memberIndex;
    std::function<void(Json::Value&)> tEvent = eventChange;
        
    this->~WrappedJsonValue(); // manually call the destructor
#pragma push_macro("new")
#undef new
    if (!tName.empty())
        new(this) WrappedJsonValue(tName, m_val);
    else if (tIndex != -1)
        new(this) WrappedJsonValue(tIndex, m_val);
    else
        assert(false);
#pragma pop_macro("new")
    eventChange = tEvent;
        
    if (changed)
        InvokeOnValueChanged();
}
    
template<typename T>
void WrappedJsonValue::Add(T&& value)
{
    assert(!m_val.isNull());
    assert(m_val.type() == Json::ValueType::arrayValue);
    assert(m_impl.childrenByIndex);
    
    Json::Value temp(value);
    int nextIndex = m_impl.childrenByIndex->size();
    
    m_impl.childrenByIndex->insert({nextIndex, WrappedJsonValue(nextIndex, temp)});
    
    m_val.append(temp);
    InvokeOnValueChanged();
}
