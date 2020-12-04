#pragma once

#include "Buildings.hpp"
#include "Building.hpp"
#include <boost/hana/for_each.hpp>
#include "Materials.hpp"
#include <unordered_map>
#include <cmath>


struct BuildingData
{
    Materials cost;
    double costMultiplier;
};

struct BuildingHash
{
    auto operator()(const Building& building) const
    {
        return std::hash<std::string>{}(building.fieldName);
    }
};

auto makeBuildingDataMap() {
    std::unordered_map<Building, BuildingData, BuildingHash> map;
    map[Building{&Buildings::metalMine}]    = {.cost = Materials{.metal = 60, .crystal = 15, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::crystalMine}] = {.cost = Materials{.metal = 48, .crystal = 24, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::deuterExtractor}] = {.cost = Materials{.metal = 225, .crystal = 75, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::solarGenerator}] = {.cost = Materials{.metal = 75, .crystal = 30, .deuter = 0}, .costMultiplier = 1.5};

    return map;
}

Materials getBuildingCost(const Building& building, int level)
{
    const static auto map = makeBuildingDataMap();
    auto it = map.find(building);
    if(it == map.end())
    {
        throw "no elo";
    }
    auto& data = it->second;
    auto& cost = data.cost;
    auto factor = pow(data.costMultiplier, level - 1);
    return Materials{.metal   = cost.metal   * factor,
                     .crystal = cost.crystal * factor,
                     .deuter  = cost.deuter  * factor};
}
