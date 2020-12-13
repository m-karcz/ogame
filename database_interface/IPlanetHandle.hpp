#pragma once

#include <memory>
#include <optional>
#include "BuildingFwd.hpp"

struct BuildingQueue;
struct Storage;
struct Buildings;

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
    virtual ~IPlanetHandle() = default;
};