#pragma once

#include "PlayerId.hpp"
#include "PlanetLocation.hpp"
#include <vector>
#include <memory>
#include "Timestamp.hpp"

struct ILazyResearchs;
struct IPlanetHandle;


struct IPlayerHandle
{
    virtual PlayerId getId() const = 0;
    virtual std::vector<PlanetLocation> getPlanetList() = 0;
    virtual std::shared_ptr<IPlanetHandle> getPlanet(const PlanetLocation&) = 0;
    virtual bool createPlanet(const PlanetLocation&, const Timestamp&) = 0;
    virtual std::shared_ptr<ILazyResearchs> getResearchs() = 0;
    virtual ~IPlayerHandle() = default;
};