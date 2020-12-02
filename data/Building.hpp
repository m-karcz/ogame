#pragma once

#include "Buildings.hpp"
#include "Conversions.hpp"

struct Building
{
    Building() : fieldName{""}, field{nullptr}
    {}
    Building(const std::string& fieldName)
        : fieldName(fieldName)
        , field(str2field<int Buildings::*>(fieldName))
    {}


    Building(int Buildings::* field)
        : fieldName(std::string{field2str(field)})
        , field{field}
    {}

    std::string fieldName;
    int Buildings::* field;

    void increment(Buildings& levels) const
    {
        ++((levels).*field);
    }
};

inline bool operator==(const Building& lhs, const Building& rhs)
{
    return lhs.field == rhs.field;
}



