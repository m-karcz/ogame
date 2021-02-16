#pragma once

#include "SinglePlanetContext.hpp"

struct ProductionInformationRequest;
struct ProductionInformationResponse;

struct ProductionInformationRequestHandler : SinglePlanetContext
{
    ProductionInformationResponse handleQuery(const ProductionInformationRequest&);
};
