#pragma once

#include "ConfigBase.h"

class BaseConfiguration
{
public:
    operator ConfVarTable&() { return *_root; }
    ConfVarTable* operator->() { return _root; }
    const ConfVarTable* operator->() const { return _root; }

protected:
    BaseConfiguration(ConfVarTable *root, bool);
    ~BaseConfiguration();

    ConfVarTable *_root;
private:
    bool _isOwner;
};


#define DEFINE_REFLECTION_BEGIN(ReflectionName)               \
    DEFINE_REFLECTION_BEGIN_(ReflectionName, BaseConfiguration)

#define DEFINE_REFLECTION_BEGIN_(ReflectionName, Base)         \
class ReflectionName : public Base                             \
{                                                              \
public:                                                        \
    ReflectionName(ConfVarTable *bindTo = nullptr, bool freeze = true);


#define DEFINE_VAR_FLOAT( var )                   ConfVarNumber &var;
#define DEFINE_VAR_INT( var )                     ConfVarNumber &var;
#define DEFINE_VAR_BOOL( var )                    ConfVarBool   &var;
#define DEFINE_VAR_STR( var )                     ConfVarString &var;
#define DEFINE_VAR_ARRAY( var )                   ConfVarArray  &var;
#define DEFINE_VAR_TABLE( var )                   ConfVarTable  &var;
#define DEFINE_VAR_REFLECTION( var, type )        type var;

#define DEFINE_REFLECTION_END()  };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
//    ConfVarNumber& InitIntType(ConfVarNumber &var, const char *type, const char *help = nullptr) { return var; }
//    ConfVarNumber& InitIntRange(ConfVarNumber &var, int iMin, int iMax, const char *help = nullptr){ return var; }
//    ConfVarNumber& InitFloat(ConfVarNumber &var, const char *help = nullptr){ return var; }
//    ConfVarBool& InitBool(ConfVarBool &var, const char *help = nullptr){ return var; }
//    ConfVarArray& InitArray(ConfVarArray &var, const char *help = nullptr){ return var; }
//    ConfVarTable& InitTable(ConfVarTable &var, const char *help = nullptr){ return var; }
//    ConfVarNumber& InitFloatRange(ConfVarNumber &var, float fMin, float fMax, const char *help = nullptr){ return var; }
//    ConfVarString& InitStrType(ConfVarString &var, const char *type, const char *help = nullptr){ return var; }

#define IMPL_REFLECTION_BEGIN_(ReflectionName, Base) ReflectionName::ReflectionName(ConfVarTable *root, bool freeze): Base(root, (false
#define IMPL_REFLECTION_BEGIN(ReflectionName) IMPL_REFLECTION_BEGIN_(ReflectionName, ReflectionBase)

#define IMPL_VAR_FLOAT( var, def )               )), var( InitFloat(_root->GetNum(#var, (float) (def))
#define IMPL_VAR_INT( var, def )               )), var( InitIntType(_root->GetNum(#var, (int) (def)), nullptr
#define IMPL_VAR_BOOL( var, def )               )), var( InitBool(_root->GetBool(#var, (def))
#define IMPL_VAR_STR( var, def )               )), var( InitStrType(_root->GetStr(#var, (def)), nullptr
#define IMPL_VAR_ARRAY( var, def )               )), var( InitArray(_root->GetArray(#var, (def))
#define IMPL_VAR_TABLE( var, def )               )), var( InitTable(_root->GetTable(#var, (def))
#define IMPL_VAR_REFLECTION( var, type )         )), var( &InitTable(_root->GetTable(#var, nullptr)

#define IMPL_REFLECTION_END()                    )) { if (freeze) _root->Freeze(true); }

