#pragma once

#include "Timestamp.hpp"
#include <sqlite_orm.h>

namespace sqlite_orm
{
    template<>
    struct type_printer<Timestamp> : integer_printer
    {
    };

    template<>
    struct statement_binder<Timestamp>
    {
        int bind(sqlite3_stmt* stmt, int index, const Timestamp& timestamp)
        {
            return statement_binder<int>().bind(stmt, index, timestamp.time_since_epoch().count());
        }
    };

    template<>
    struct field_printer<Timestamp>
    {
        std::string operator()(const Timestamp& timestamp)
        {
            return std::to_string(timestamp.time_since_epoch().count());
        }
    };

    template<>
    struct row_extractor<Timestamp>
    {
        Timestamp extract(const char* rowValue)
        {
            return Timestamp{Duration{std::stoi(rowValue)}};
        }

        Timestamp extract(sqlite3_stmt* stmt, int colIndex)
        {
            auto str = sqlite3_column_text(stmt, colIndex);
            return this->extract((const char*)str);
        }
    };


}

