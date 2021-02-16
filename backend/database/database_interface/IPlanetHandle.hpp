#pragma once

#include <memory>
#include <optional>
#include "BuildingFwd.hpp"

struct BuildingQueue;
struct Storage;
struct Buildings;
struct CachedProduction;
struct ProductionPercentages;

struct IPlanetHandle
{
    virtual Buildings getBuildings() = 0;
    virtual std::optional<BuildingQueue> getBuildingQueue() = 0;
    virtual void incrementBuildingLevel(const Building&) = 0;
    virtual Storage getStorage() = 0;
    virtual void setNewStorage(const Storage&) = 0;
    virtual int getBuildingLevel(const Building&) = 0;
    virtual void queueBuilding(const BuildingQueue&) = 0;
    virtual void dequeueBuilding(const BuildingQueue&) = 0;
    virtual CachedProduction getCachedProduction() = 0;
    virtual void setNewCachedProduction(const CachedProduction&) = 0;
    virtual ProductionPercentages getProductionPercentages() = 0;
    virtual void setNewProductionPercentages(const ProductionPercentages&) = 0;
    virtual ~IPlanetHandle() = default;
};