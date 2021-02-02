#include "Buildings.hpp"
#include <boost/hana/define_struct.hpp>
#include "Constraints.hpp"
#include <sqlite_orm.h>
#include "PlanetWrapper.hpp"
#include "NamedColumn.hpp"

namespace sqlite
{
    struct BuildingsWrapper : Buildings
    {
        using Base = Buildings;
        BOOST_HANA_DEFINE_STRUCT(BuildingsWrapper,
                                (int, planetId));
    };

    template<> auto columnOptions<&BuildingsWrapper::planetId> = std::make_tuple(sqlite_orm::primary_key(), sqlite_orm::unique());

    template<>
    auto constraints<BuildingsWrapper>()
    {
        return std::make_tuple(sqlite_orm::foreign_key(&BuildingsWrapper::planetId).references(&PlanetWrapper::planetId));
    }
}

template auto makeTable<sqlite::BuildingsWrapper>(std::string);