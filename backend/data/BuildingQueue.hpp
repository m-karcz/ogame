#pragma once

#include "Timestamp.hpp"
#include "Building.hpp"
#include <boost/hana/define_struct.hpp>

struct BuildingQueue
{
    BOOST_HANA_DEFINE_STRUCT(BuildingQueue,
                             (Building, building),
                             (Timestamp, finishAt));
};