#pragma once

#include "SinglePlanetContext.hpp"

struct ProductionPercentagesRequest;
struct ProductionPercentagesResponse;

struct ProductionPercentagesRequestHandler : SinglePlanetContext
{
    ProductionPercentagesResponse handleQuery(const ProductionPercentagesRequest&);
};
