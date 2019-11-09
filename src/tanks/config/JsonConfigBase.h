#pragma once

#include "json.h"

#include <map>
#include <unordered_map>
#include <vector>

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <deque>

class JsonConfigValue
{
public:
    void Print() const
    {
        std::cout << _val.toStyledString() << std::endl;
    }

private:
    static Json::Value nullV;
    static JsonConfigValue null;
    
    
public:
    
    JsonConfigValue();
    JsonConfigValue(const std::string& memberName, Json::Value& value);
    JsonConfigValue(int memberIndex, Json::Value& value);
    JsonConfigValue(JsonConfigValue&& o) noexcept;
    ~JsonConfigValue();
    
    template<typename T> T Get() const;
    
    template<> bool Get() const;
    template<> int Get() const;
    template<> float Get() const;
    template<> double Get() const;
    template<> std::string Get() const;
    
    template<> const std::map<int, JsonConfigValue>& Get() const;
    
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
    
    void Remove(const std::string& name);
    void Remove(int index);
    
    JsonConfigValue& GetValueByPath(const std::string& path);
    const JsonConfigValue& GetValueByPath(const std::string& path) const;
    
    JsonConfigValue& operator[](const std::string &path);
    const JsonConfigValue& operator[](const std::string &path) const;
    
    JsonConfigValue& operator[](int index);
    JsonConfigValue& operator=(const JsonConfigValue& other);
    
    bool IsNull() const { return this == &null; }
    
    std::function<void(Json::Value&)> eventChange;
private:
    JsonConfigValue(const JsonConfigValue& o);
    void swap( JsonConfigValue &other );
    
    template<typename T>
    void Set(T&& value);
        
    template<typename T>
    void Add(T&& value);
    
    std::vector<std::string> split(const std::string& ss) const;
    void ExpandFrom(Json::Value& value);
    void InvokeOnValueChanged();
    
    union Data
    {
        std::map<std::string, JsonConfigValue>*   _childrenByName;
        std::map<int, JsonConfigValue>*           _childrenByIndex;
    };
private:
    Json::Value& _val;
    std::string _memberName;
    int         _memberIndex { 0 };
    Data        _impl;
};

template<typename T>
void JsonConfigValue::Set(T&& value)
{
    assert(!_val.isNull());
    assert(!_memberName.empty());
        
    Json::Value temp(value);
    bool changed = temp != _val;
    _val = value;
        
    std::string tName = _memberName;
    std::function<void(Json::Value&)> tEvent = eventChange;
        
    this->~JsonConfigValue(); // manually call the destructor
#pragma push_macro("new")
#undef new
    new(this) JsonConfigValue(tName, _val);
#pragma pop_macro("new")
    eventChange = tEvent;
        
    if (changed)
        InvokeOnValueChanged();
}
    
template<typename T>
void JsonConfigValue::Add(T&& value)
{
    assert(!_val.isNull());
    assert(_val.type() == Json::ValueType::arrayValue);
    assert(_impl._childrenByIndex);
    
    Json::Value temp(value);
    int nextIndex = _impl._childrenByIndex->size();
    
    _impl._childrenByIndex->insert({nextIndex, JsonConfigValue(nextIndex, temp)});
    
    _val.append(temp);
    InvokeOnValueChanged();
}

class JsonConfig
{
public:
    JsonConfig(Json::Value& value)
    :_val(value)
    ,_root("root", _val)
    {
    }
    
    static void Load(const char* fileName)
    {
        const char* str = R"(
{
    "test":{
        "str": "2020303",
        "int": 1010,
        "arr":[ 1, 2, 3, 4 ]
    },
        
    "tiers": [
      {
        "title": "Tier 1",
        "maps": [
                  {
                    "map_name": "dm1",
                    "timelimit": 2,
                    "fraglimit": 5,
                    "bot_names": [ "Insert" ]
                  },
                  {
                    "map_name": "dm2",
                    "timelimit": 3,
                    "fraglimit": 10,
                    "bot_names": [ "Insert", "Sulik" ]
                  },
                  {
                    "map_name": "dm3",
                    "timelimit": 3,
                    "fraglimit": 10,
                    "bot_names": [ "Insert", "Sulik" ]
                  },
                  {
                    "map_name": "dm5",
                    "timelimit": 10,
                    "fraglimit": 15,
                    "bot_names": [ "Insert", "Sulik", "Frank" ]
                  }
        ]
      }
    ]
}
                            )";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
        
        JsonConfig config(val2);
        JsonConfigValue& val = config.GetValueByPath("test,arr");
    
        val.eventChange = [](Json::Value& val) { std::cout << val.toStyledString() << std::endl; };
        val[3].Print();
        
        //val.Set(10);
        val.Add(10);
        config.Print();
        config.GetValueByPath("test").Remove("str");
        config.GetValueByPath("test").Remove("int");
        
        val.Remove(0);
        val.Remove(2);
       // config.GetValueByPath("test").Remove("arr");
        
            
        config.Print();
    }
    
    JsonConfigValue& GetValueByPath(const std::string& path)
    {
        return _root.GetValueByPath(path);
    }
    
    void Print() const
    {
        _root.Print();
    }
private:
    Json::Value _val;
    JsonConfigValue _root;
};
