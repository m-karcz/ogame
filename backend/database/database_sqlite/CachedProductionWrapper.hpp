#pragma once
#include "CachedProduction.hpp"
#include "Constraints.hpp"
#include <sqlite_orm.h>
#include "PlanetWrapper.hpp"

namespace sqlite
{
    struct CachedProductionWrapper : CachedProduction
    {
        using Base = CachedProduction;
        BOOST_HANA_DEFINE_STRUCT(CachedProductionWrapper,
                                 (int, planetId));
    };

    template<> auto columnOptions<&CachedProductionWrapper::planetId> = std::make_tuple(sqlite_orm::primary_key(), sqlite_orm::unique());
    template<>
    auto constraints<CachedProductionWrapper>()
    {
        return std::make_tuple(sqlite_orm::foreign_key(&CachedProductionWrapper::planetId).references(&PlanetWrapper::planetId));
    }
}