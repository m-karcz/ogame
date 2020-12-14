#pragma once

#include "SinglePlanetContext.hpp"

struct BuildRequest;
struct BuildResponse;

struct BuildRequestHandler : SinglePlanetContext
{
    BuildResponse handleAction(const BuildRequest&);
};
