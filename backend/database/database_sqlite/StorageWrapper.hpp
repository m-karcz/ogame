#pragma once

#include "Storage.hpp"
#include <boost/hana/define_struct.hpp>
#include "Constraints.hpp"
#include <sqlite_orm.h>
#include "PlanetWrapper.hpp"
#include "NamedColumn.hpp"

namespace sqlite
{
    struct StorageWrapper : Storage
    {
        using Base = Storage;
        BOOST_HANA_DEFINE_STRUCT(StorageWrapper,
                                 (int, planetId));
    };

    template<> auto columnOptions<&StorageWrapper::planetId> = std::make_tuple(sqlite_orm::primary_key(), sqlite_orm::unique());
    template<>
    auto constraints<StorageWrapper>()
    {
        return std::make_tuple(sqlite_orm::foreign_key(&StorageWrapper::planetId).references(&PlanetWrapper::planetId));
    }
}
template auto makeTable<sqlite::StorageWrapper>(std::string);