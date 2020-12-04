#pragma once

#include "BuildingQueueRequest.hpp"
#include "Building.hpp"
#include <boost/hana/define_struct.hpp>

struct BuildingQueueEntry
{
    BOOST_HANA_DEFINE_STRUCT(BuildingQueueEntry,
                             (Building,building),
                             (Duration,timeToFinish));
};

struct BuildingQueueRequest
{
    BOOST_HANA_DEFINE_STRUCT(BuildingQueueRequest);
};

struct BuildingQueueResponse
{
    BOOST_HANA_DEFINE_STRUCT(BuildingQueueResponse,
                             (std::optional<BuildingQueueEntry>, queue));
};