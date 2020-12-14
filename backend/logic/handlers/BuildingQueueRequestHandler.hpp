#pragma once

#include "SinglePlanetContext.hpp"

struct BuildingQueueRequest;
struct BuildingQueueResponse;

struct BuildingQueueRequestHandler : SinglePlanetContext
{
    BuildingQueueResponse handleQuery(const BuildingQueueRequest&);
};
