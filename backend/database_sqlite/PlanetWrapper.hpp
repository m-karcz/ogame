#pragma once

#include <boost/hana/define_struct.hpp>
#include "PlanetLocation.hpp"
#include "Constraints.hpp"
#include "ColumnOptions.hpp"
#include "UserCredentialsWrapper.hpp"
#include <sqlite_orm.hpp>

namespace sqlite
{

struct PlanetWrapper : PlanetLocation
{
    using Base = PlanetLocation;
    BOOST_HANA_DEFINE_STRUCT(PlanetWrapper,
                             (int, playerId),
                             (int, planetId));

};

template<> auto columnOptions<&PlanetWrapper::planetId> = std::make_tuple(sqlite_orm::autoincrement(), sqlite_orm::primary_key());

template<>
auto constraints<PlanetWrapper>()
{
    return std::make_tuple(sqlite_orm::foreign_key(&PlanetWrapper::playerId).references(&UserCredentialsWrapper::id));
}

}