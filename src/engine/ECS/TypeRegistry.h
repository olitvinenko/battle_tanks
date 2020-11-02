#pragma once

#include <cstddef>

using type_id_t = std::size_t;

namespace Internal
{
    template <typename T>
    class TypeFamily
    {
    private:
        template <typename U>
        struct type { static void id() { } };
    public:
        static type_id_t GetId();
    };

    template <typename T>
    type_id_t TypeFamily<T>::GetId()
    {
        static type_id_t typeId = reinterpret_cast<type_id_t>(&type<T>::id);
        return typeId;
    }
}

#define ECS_DECLARE_TYPE static type_id_t GetStaticId();
#define ECS_DEFINE_TYPE(TypeName) type_id_t TypeName::GetStaticId() { return Internal::TypeFamily<TypeName>::GetId(); }

template<typename T, typename = decltype(T::GetStaticId)>
type_id_t GetTypeIndex()
{
    return T::GetStaticId();
}
