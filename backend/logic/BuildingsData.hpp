#pragma once

#include "Buildings.hpp"
#include "Building.hpp"
#include <boost/hana/for_each.hpp>
#include "Materials.hpp"
#include <unordered_map>
#include <cmath>
#include "Logger.hpp"


struct BuildingData
{
    Materials cost;
    double costMultiplier;
};

inline auto makeBuildingDataMap() {
    std::unordered_map<Building, BuildingData> map;
#define AAA(name, metalCost, crystalCost, deuterCost, multiplierCost) map[Building{&Buildings::name}] = {.cost = Materials{.metal = metalCost, .crystal = crystalCost, .deuter = deuterCost}, .costMultiplier = multiplierCost};
        //building name
    AAA(metalMine, 60, 15, 0, 1.5);
    //map[Building{&Buildings::metalMine}]    = {.cost = Materials{.metal = 60, .crystal = 15, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::crystalMine}] = {.cost = Materials{.metal = 48, .crystal = 24, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::deuteriumSynthesizer}] = {.cost = Materials{.metal = 225, .crystal = 75, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::solarPlant}] = {.cost = Materials{.metal = 75, .crystal = 30, .deuter = 0}, .costMultiplier = 1.5};
    map[Building{&Buildings::fusionReactor}] = {.cost = Materials{.metal = 900, .crystal = 360, .deuter = 180}, .costMultiplier = 1.8};
    map[Building{&Buildings::roboticsFactory}] = {.cost = Materials{.metal = 400, .crystal = 120, .deuter = 200}, .costMultiplier = 2.0};
    map[Building{&Buildings::shipyard}] = {.cost = Materials{.metal = 400, .crystal = 200, .deuter = 100}, .costMultiplier = 2.0};
    map[Building{&Buildings::naniteFactory}] = {.cost = Materials{.metal = 1'000'000, .crystal = 500'000, .deuter = 100'000}, .costMultiplier = 2.0};
    map[Building{&Buildings::metalStorage}] = {.cost = Materials{.metal = 1000, .crystal = 0, .deuter = 0}, .costMultiplier = 2.0};
    map[Building{&Buildings::crystalStorage}] = {.cost = Materials{.metal = 1000, .crystal = 500, .deuter = 0}, .costMultiplier = 2.0};
    map[Building{&Buildings::deuteriumTank}] = {.cost = Materials{.metal = 1000, .crystal = 1000, .deuter = 0}, .costMultiplier = 2.0};
#undef AAA
    return map;
}

inline Materials getBuildingCost(const Building& building, int level)
{
    const static auto map = makeBuildingDataMap();
    auto it = map.find(building);
    if(it == map.end())
    {
        throw "no elo";
    }
    auto& data = it->second;
    auto& cost = data.cost;
    auto factor = pow(data.costMultiplier, level);
    logger.debug("raw cost: {}, {}, {}", cost.metal.toString(), cost.crystal.toString(), cost.deuter.toString());
    logger.debug("factor: {}", factor);
    return Materials{.metal   = cost.metal   * factor,
                     .crystal = cost.crystal * factor,
                     .deuter  = cost.deuter  * factor};
}
