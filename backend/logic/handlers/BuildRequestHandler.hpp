#pragma once

#include "SinglePlanetContext.hpp"
#include "Duration.hpp"

struct BuildRequest;
struct BuildResponse;
struct Cost;

struct BuildRequestHandler : SinglePlanetContext
{
    BuildResponse handleAction(const BuildRequest&);
private:
    Duration timeToBuild(const Cost&) const;
};
