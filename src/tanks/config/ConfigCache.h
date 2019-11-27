// ConfigCache.h
// this file is designed to be included twice

#include "ConfigBase.h"
#include "JsonConfig.h"

#include "FileSystem.h"

#include <cassert>


#ifndef CONFIG_CACHE_PASS2

#ifndef CONFIG_CACHE_PASS1_INCLUDED
#define CONFIG_CACHE_PASS1_INCLUDED

    namespace config_detail
    {
        class JsonReflectionBase
        {
        public:
            JsonReflectionBase(std::shared_ptr<FileSystem::Stream> stream)
                : m_config(JsonConfig::MakeFrom(stream->GetContent()))
            {
            }
            
        protected:
            std::shared_ptr<JsonConfig> m_config;
        };
    
        class ReflectionBase
        {
        public:
            operator ConfVarTable&() { return *_root; }
            ConfVarTable* operator->() { return _root; }
            const ConfVarTable* operator->() const { return _root; }

        protected:
            ReflectionBase(ConfVarTable *root, bool)
                : _root(root)
                , _isOwner(root == nullptr)
            {
                if (_isOwner)
                {
                    assert(!_root);
                    _root = new ConfVarTable();
                }
            }
            ~ReflectionBase()
            {
                if (_isOwner)
                {
                    assert(_root);
                    delete _root;
                }
            }

            ConfVarTable *_root;

        private:
            bool _isOwner;
        };
    }

#define REFLECTION_BEGIN(ReflectionName)                           \
    REFLECTION_BEGIN_(ReflectionName, config_detail::ReflectionBase)

#define REFLECTION_BEGIN_(ReflectionName, Base)                    \
    class ReflectionName : public Base                             \
    {                                                              \
    public:                                                        \
        ReflectionName(ConfVarTable *bindTo = nullptr, bool freeze = true);


 #define VAR_FLOAT( var, def )                   ConfVarNumber &var;
 #define VAR_INT(   var, def )                   ConfVarNumber &var;
 #define VAR_BOOL(  var, def )                   ConfVarBool   &var;
 #define VAR_STR(   var, def )                   ConfVarString &var;
 #define VAR_ARRAY( var, def )                   ConfVarArray  &var;
 #define VAR_TABLE( var, def )                   ConfVarTable  &var;
 #define VAR_REFLECTION( var, type )             type var;

#define REFLECTION_END()  };
#endif //CONFIG_CACHE_PASS_1_INCLUDED

///////////////////////////////////////////////////////////////////////////////
#else // CONFIG_CACHE_PASS2
///////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_CACHE_PASS2_INCLUDED
#define CONFIG_CACHE_PASS2_INCLUDED

#undef REFLECTION_BEGIN
#undef REFLECTION_BEGIN_
 #undef VAR_FLOAT
 #undef VAR_INT
 #undef VAR_BOOL
 #undef VAR_STR
 #undef VAR_ARRAY
 #undef VAR_TABLE
 #undef VAR_REFLECTION
#undef REFLECTION_END

#define REFLECTION_BEGIN_(ReflectionName, Base) ReflectionName::ReflectionName(ConfVarTable *root, bool freeze): Base(root, (false
#define REFLECTION_BEGIN(ReflectionName) REFLECTION_BEGIN_(ReflectionName, config_detail::ReflectionBase)

#define VAR_FLOAT( var, def )               )), var( _root->GetNum(#var, (float) (def)
#define VAR_INT(   var, def )               )), var( _root->GetNum(#var, (int) (def)
#define VAR_BOOL(  var, def )               )), var( _root->GetBool(#var, (def)
#define VAR_STR(   var, def )               )), var( _root->GetStr(#var, (def)
#define VAR_ARRAY( var, def )               )), var( _root->GetArray(#var, (def)
#define VAR_TABLE( var, def )               )), var( _root->GetTable(#var, (def)
#define VAR_REFLECTION( var, type )         )), var( &_root->GetTable(#var, nullptr

#define REFLECTION_END()                    )) { if (freeze) _root->Freeze(true); }

#endif // CONFIG_CACHE_PASS2_INCLUDED
#endif // CONFIG_CACHE_PASS2
