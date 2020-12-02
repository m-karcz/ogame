#include "BuildingQueue.hpp"
#include <boost/hana/define_struct.hpp>
#include "Constraints.hpp"
#include <sqlite_orm.hpp>
#include "PlanetWrapper.hpp"

namespace sqlite
{
    struct BuildingQueueWrapper : BuildingQueue
    {
        using Base = BuildingQueue;
        BOOST_HANA_DEFINE_STRUCT(BuildingQueueWrapper,
                                 (int, planetId));
    };

    template<> auto columnOptions<&BuildingQueueWrapper::planetId> = std::make_tuple(sqlite_orm::primary_key(), sqlite_orm::unique());
    template<>
    auto constraints<BuildingQueueWrapper>()
    {
        return std::make_tuple(sqlite_orm::foreign_key(&BuildingQueueWrapper::planetId).references(&PlanetWrapper::planetId));
    }
}
