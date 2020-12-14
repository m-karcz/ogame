#pragma once

#include <boost/hana/define_struct.hpp>
#include "PlanetLocation.hpp"
#include "Storage.hpp"

struct StorageRequest
{
    BOOST_HANA_DEFINE_STRUCT(StorageRequest,
                             (PlanetLocation, planet));
};

struct StorageResponse
{
    BOOST_HANA_DEFINE_STRUCT(StorageResponse,
                             (Storage, storage));
};