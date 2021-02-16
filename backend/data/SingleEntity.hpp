#pragma once
#include "Conversions.hpp"
#include "SingleEntityFwd.hpp"
#include <string>

template<typename Parent>
struct SingleEntity
{
    SingleEntity() : fieldName{""}, field{nullptr}
    {}
    SingleEntity(const std::string& fieldName)
            : fieldName(fieldName)
            , field(str2field<int Parent::*>(fieldName))
    {}


    SingleEntity(int Parent::* field)
            : fieldName(std::string{field2str(field)})
            , field{field}
    {}

    std::string fieldName;
    int Parent::* field;

    void increment(Parent& levels) const
    {
        ++((levels).*field);
    }
};

template<typename Parent>
inline bool operator==(const SingleEntity<Parent>& lhs, const SingleEntity<Parent>& rhs)
{
    return lhs.field == rhs.field;
}

namespace std
{
    template<typename Parent>
    struct hash<SingleEntity<Parent>>
    {
        auto operator()(const SingleEntity<Parent>& entity) const
        {
            return std::hash<std::string>{}(entity.fieldName);
        }
    };
}