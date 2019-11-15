#pragma once
 
#include "json.h"
#include "WrappedJsonValue.h"

#include <map>
#include <unordered_map>
#include <vector>

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <deque>

class JsonConfig2
{
public:
    JsonConfig2(Json::Value& value)
    :_val(value)
    ,_root("root", _val)
    {
    }
    
    static void TestGetInt()
    {
        const char* str = R"({
            "var": 100500
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        assert(!val.IsNull());
        config.Print();
    }
    
    static void TestGetInnerInt()
    {
        const char* str = R"({
            "var": {
                "innerVar": 1050
            }
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var,innerVar");
        assert(!val.IsNull());
        config.Print();
    }
    
    static void TestIntOnChange()
    {
        const char* str = R"({
            "var": 100500
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        bool changed = false;
        val.eventChange = [&changed](Json::Value& val) {
            changed = true;
            std::cout << val.toStyledString() << std::endl;
        };
        
        val.Set(10);
        
        auto var = val.Get<int>();
        assert(changed && var == 10);
        config.Print();
    }
    
    static void TestInnerIntOnChange()
    {
        const char* str = R"({
            "var": {
                "innerVar": 1050
            }
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var,innerVar");
        
        bool changed = false;
        val.eventChange = [&changed](Json::Value& val) {
            changed = true;
            std::cout << val.toStyledString() << std::endl;
        };
        
        val.Set(10);
        
        auto var = val.Get<int>();
        assert(changed && var == 10);
        config.Print();
    }
    
    static void TestGetArray()
    {
        const char* str = R"({
            "var": [1, 2, 3, 4, 5]
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        assert(!val.IsNull());
    }
    
    static void TestRemoveFromArray()
    {
        const char* str = R"({
            "var": [1, 2, 3, 4, 5]
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        bool changed = false;
        val.eventChange = [&changed](Json::Value& val) {
            changed = true;
            std::cout << val.toStyledString() << std::endl;
        };
        
        assert(val.Remove(0));
        assert(changed);
        
        changed = false;
        assert(val.Remove(0));
        assert(changed);
        
        config.Print();
    }
    
    static void TestAddToArray()
    {
        const char* str = R"({
            "var": [1, 2, 3, 4, 5]
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        bool changed = false;
        val.eventChange = [&changed](Json::Value& val) {
            changed = true;
            std::cout << val.toStyledString() << std::endl;
        };
        
        val.Add(100400);
        val.Add(true);
        assert(changed);
        
        config.Print();
    }
    
    static void TestArrayIndexer()
    {
        const char* str = R"({
            "var": [1, 2, 3, 4, 5]
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        WrappedJsonValue& zero = val[0];
        WrappedJsonValue& first = val[1];
        
        assert(!zero.IsNull());
        assert(!first.IsNull());
        
        zero.eventChange = [](Json::Value& val) { std::cout << val.toStyledString() << std::endl; };
        zero.Set(10);
        zero.Set(100);
        
        zero.Print();
        first.Print();
        
        config.Print();
    }
    
    static void TestAssignOperator()
    {
        const char* str = R"({
            "var": [1, 2, 3, 4, 5]
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        WrappedJsonValue& zero = val[0];
        WrappedJsonValue& first = val[1];
        
        assert(!zero.IsNull());
        assert(!first.IsNull());
        
        zero.eventChange = [](Json::Value& val) { std::cout << val.toStyledString() << std::endl; };
        zero = first;
        
        zero.Print();
        first.Print();
        
        zero.Set(53);
        first.Set(1040);
        
        config.Print();
    }
    
    static void TestRemoveMemberByName()
    {
        const char* str = R"({
            "var": {
                "member1": 1,
                "member2": 2,
                "member3": {
                    "iM": 10
                }
            }
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        assert(!val.IsNull());

        val.eventChange = [](Json::Value& val) { std::cout << val.toStyledString() << std::endl; };

        val.Remove("member3");
        
        config.Print();
    }
    
    static void TestTrivialAssignOps()
    {
        const char* str = R"({
            "var": 1
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        val.eventChange = [](Json::Value& val) { std::cout << val.toStyledString() << std::endl; };
        
        val = 10;
        val = 11.0f;
        val = 20.0;
        val = "abc";
        val = false;
        val = val2;
        
        config.Print();
    }
    
    static void TestComplexCases()
    {
        const char* str = R"({
            "var": 1
        })";
        
        Json::Value val2;
        Json::Reader rd;
        if (!rd.parse(str, val2))
            return;
            
        JsonConfig2 config(val2);
        WrappedJsonValue& val = config.GetValueByPath("var");
        
        val.eventChange = [](Json::Value& val) { std::cout << val.toStyledString() << std::endl; };
        
        val = 10;
        val = 11.0f;
        val = 20.0;
        val = "abc";
        val = false;
        val = val2;
        
        WrappedJsonValue& val3 = val.GetValueByPath("var");
        val3.Set(10.9);
        
        config.Print();
    }
    
    static void Load(const char* fileName)
    {
        TestGetInt();
        TestGetInnerInt();
        TestIntOnChange();
        TestInnerIntOnChange();
        
        TestGetArray();
        TestRemoveFromArray();
        TestAddToArray();
        TestArrayIndexer();
        TestAssignOperator();
        
        TestRemoveMemberByName();
        TestTrivialAssignOps();
        
        TestComplexCases();
    }
    
    WrappedJsonValue& GetValueByPath(const std::string& name)
    {
        return _root.GetValueByPath(name);
    }
    
    void Print() const
    {
        _root.Print();
    }
private:
    Json::Value _val;
    WrappedJsonValue _root;
};
