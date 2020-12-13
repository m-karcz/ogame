#pragma once

#include "SinglePlanetContext.hpp"

struct BuildingsListRequest;
struct BuildingsListResponse;

struct BuildingsListRequestHandler : SinglePlanetContext
{
    BuildingsListResponse handleQuery(const BuildingsListRequest&);
};
