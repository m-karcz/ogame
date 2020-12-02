#pragma once

#include "PlanetLocation.hpp"
#include "StorageRequest.hpp"
#include "BuildRequest.hpp"
#include "BuildingsListRequest.hpp"
#include <vector>
#include "PlayerId.hpp"
#include <boost/hana/define_struct.hpp>

struct OnPlanetRequest
{
    using Variant = std::variant<StorageRequest, BuildRequest, BuildingsListRequest>;
    BOOST_HANA_DEFINE_STRUCT(OnPlanetRequest,
                             (PlayerId,playerId),
                             (PlanetLocation,planet),
                             (std::vector<Variant>,requests));
};

using OnPlanetResponse = std::vector<std::variant<StorageResponse, BuildResponse, BuildingsListResponse>>;