#pragma once

#include "SinglePlanetContext.hpp"

struct StorageRequest;
struct StorageResponse;

struct StorageRequestHandler : SinglePlanetContext
{
    StorageResponse handleQuery(const StorageRequest&);
};