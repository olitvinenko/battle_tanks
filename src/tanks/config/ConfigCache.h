// ConfigCache.h
// this file is designed to be included twice

#include "VariableTable.h"

#include <cassert>

#ifndef CONFIG_CACHE_PASS2

#ifndef CONFIG_CACHE_PASS1_INCLUDED
#define CONFIG_CACHE_PASS1_INCLUDED

    namespace config_detail
    {
        class ReflectionBase
        {
        public:
            operator VariableTable&() const { return *m_root; }
            VariableTable* operator->() { return m_root; }
            const VariableTable* operator->() const { return m_root; }

        protected:
            ReflectionBase(VariableTable *root)
                : m_root(root)
                , m_isOwner(root == nullptr)
            {
                if (m_isOwner)
                {
                    assert(!m_root);
                    m_root = new VariableTable();
                }
            }
            ~ReflectionBase()
            {
                if (m_isOwner)
                {
                    assert(m_root);
                    delete m_root;
                }
            }

            VariableTable *m_root;
        private:
            bool m_isOwner;
        };
    }

#define REFLECTION_BEGIN(ReflectionName)                           \
    REFLECTION_BEGIN_(ReflectionName, config_detail::ReflectionBase)

#define REFLECTION_BEGIN_(ReflectionName, Base)                    \
    class ReflectionName : public Base                             \
    {                                                              \
    public:                                                        \
        ReflectionName(VariableTable *bindTo = nullptr);

#include "VariableBool.h"
#include "VariableNumber.h"
#include "VariableString.h"
#include "VariableArray.h"


 #define VAR_FLOAT( var, def )              VariableNumber &var;
 #define VAR_INT(   var, def )              VariableNumber &var;
 #define VAR_BOOL(  var, def )              VariableBool   &var;
 #define VAR_STR(   var, def )              VariableString &var;
 #define VAR_ARRAY( var, def )              VariableArray  &var;
 #define VAR_TABLE( var, def )              VariableTable  &var;
 #define VAR_REFLECTION( var, type )        type var;

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

#define REFLECTION_BEGIN_(ReflectionName, Base) ReflectionName::ReflectionName(VariableTable *root): Base(root
#define REFLECTION_BEGIN(ReflectionName) REFLECTION_BEGIN_(ReflectionName, config_detail::ReflectionBase)

#define VAR_FLOAT( var, def )          ), var( m_root->GetNum(#var, (float) (def))
#define VAR_INT(   var, def )          ), var( m_root->GetNum(#var, (int) (def))
#define VAR_BOOL(  var, def )          ), var( m_root->GetBool(#var, (def))
#define VAR_STR(   var, def )          ), var( m_root->GetStr(#var, (def))
#define VAR_ARRAY( var, def )          ), var( m_root->GetArray(#var, (def))
#define VAR_TABLE( var, def )          ), var( m_root->GetTable(#var, (def))
#define VAR_REFLECTION( var, type )    ), var( &m_root->GetTable(#var, nullptr)

#define REFLECTION_END()                    ) {  }

#endif // CONFIG_CACHE_PASS2_INCLUDED
#endif // CONFIG_CACHE_PASS2
