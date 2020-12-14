#pragma once

#include "PlanetLocation.hpp"
#include "api/StorageRequest.hpp"
#include "BuildRequest.hpp"
#include "api/BuildingsListRequest.hpp"
#include "api/BuildingQueueRequest.hpp"
#include <vector>
#include "PlayerId.hpp"
#include <boost/hana/define_struct.hpp>

using OnPlanetAction = std::variant<BuildRequest>;
using OnPlanetQuery = std::variant<StorageRequest, BuildingsListRequest, BuildingQueueRequest>;

struct OnPlanetRequest
{
    BOOST_HANA_DEFINE_STRUCT(OnPlanetRequest,
    (PlayerId,playerId),
    (PlanetLocation,planet),
    (std::vector<OnPlanetQuery>,queries),
    (std::optional<OnPlanetAction>,action));
};

using OnPlanetResponse = std::vector<std::variant<StorageResponse, BuildResponse, BuildingsListResponse, BuildingQueueResponse>>;
