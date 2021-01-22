#pragma once

#include "BigNum.hpp"
#include <sqlite_orm.h>
#include "Logger.hpp"

namespace sqlite_orm
{
    template<>
    struct type_printer<BigNum> : integer_printer
    {
    };

    template<>
    struct statement_binder<BigNum>
    {
        int bind(sqlite3_stmt* stmt, int index, const BigNum& value)
        {
            logger.debug("{} {}", value.toString(), __PRETTY_FUNCTION__);
            return statement_binder<std::string>().bind(stmt, index, value.toString());
        }
    };

    template<>
    struct field_printer<BigNum>
    {
        std::string operator()(const BigNum& value)
        {
            logger.debug("{} {}", value.toString(), __PRETTY_FUNCTION__);
            return value.toString();
        }
    };

    template<>
    struct row_extractor<BigNum>
    {
        BigNum extract(const char* rowValue)
        {
            logger.debug("{} {}", rowValue, __PRETTY_FUNCTION__);
            logger.debug("{} {}", BigNum::fromStr(rowValue).toString(), __PRETTY_FUNCTION__);
            return BigNum::fromStr(rowValue);
        }

        BigNum extract(sqlite3_stmt* stmt, int colIndex)
        {
            auto str = sqlite3_column_text(stmt, colIndex);
            return this->extract((const char*)str);
        }
    };
}