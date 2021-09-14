#pragma once

#include "PrimaryKeyed.hpp"
#include "sqlite3.h"
#include <boost/hana/for_each.hpp>
#include <boost/hana/accessors.hpp>
#include <string>
#include "Serialization.hpp"
#include "GetTypeName.hpp"
#include "Logger.hpp"

namespace sqlitedb
{
template<typename T>
void makePrimaryKeyedTable(sqlite3* handle, std::string tableName = std::string{getTypeName<T>()}, bool autoincrement = false)
{
    using namespace boost;
    std::string query = "CREATE TABLE IF NOT EXISTS";

    query += " ";
    query += tableName;
    query += "(";
    query += "id INTEGER PRIMARY KEY";

    if(autoincrement)
    {
        query += " AUTOINCREMENT";
    }
    
    hana::for_each(hana::accessors<T>(), [&](auto accessor){
        query += ", ";
        query += hana::first(accessor).c_str();
        query += " ";
        query += serializationRepresentation<decltype(hana::second(accessor)(std::declval<T>()))>();
    });

    query += ");";

    logger.debug(query);

    sqlite3_exec(handle,
                    query.c_str(),
                    +[](void*, int, char**, char**)->int{
                        return 0;
                    },
                    nullptr,
                    nullptr);
}

}