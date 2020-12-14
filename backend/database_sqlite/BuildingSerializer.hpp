#pragma once

#include "Building.hpp"
#include <sqlite_orm.hpp>

namespace sqlite_orm
{
    template<>
    struct type_printer<Building> : text_printer
    {
    };

    template<>
    struct statement_binder<Building>
    {
        int bind(sqlite3_stmt* stmt, int index, const Building& value)
        {
            return statement_binder<std::string>().bind(stmt, index, value.fieldName);
        }
    };

    template<>
    struct field_printer<Building>
    {
        std::string operator()(const Building& value)
        {
            return value.fieldName;
        }
    };

    template<>
    struct row_extractor<Building>
    {
        Building extract(const char* rowValue)
        {
            return {{rowValue}};
        }

        Building extract(sqlite3_stmt* stmt, int colIndex)
        {
            auto str = sqlite3_column_text(stmt, colIndex);
            return this->extract((const char*)str);
        }
    };
}