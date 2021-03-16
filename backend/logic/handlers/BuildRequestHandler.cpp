#include "BuildRequest.hpp"
#include "BuildResponse.hpp"
#include "BuildRequestHandler.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"
#include "Building.hpp"
#include "Buildings.hpp"
#include "BuildingsData.hpp"
#include "BuildingQueue.hpp"
#include "KnowledgeData.hpp"
#include "Constants.hpp"
#include "Configuration.hpp"
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace
{
bool areRequirementsSatisfied(const Building& buidlingToCheck, const Buildings& buildings, const Researchs& researchs)
{
    auto reqs = ranges::find_if(knowledgeData.requirements.buildings, 
                                [&](auto& req){return req.name == buidlingToCheck;});
    if(reqs == knowledgeData.requirements.buildings.end())
    {
        throw "no elo 123";
    }
    return ranges::all_of(reqs->requirements.buildings,
                         [&](auto& req){return req.name.value(buildings) >= req.level;})
           and
           ranges::all_of(reqs->requirements.researchs,
                          [&](auto& req){return req.name.value(researchs) >= req.level;});
}
}

BuildResponse BuildRequestHandler::handleAction(const BuildRequest& req)
{
    logger.debug(__PRETTY_FUNCTION__);
    Building building{req.buildingName};

    logger.debug("Got building to built: {}", building.fieldName);
    int level = planet.getBuildingLevel(building);

    auto cost = getBuildingCost(building, level);

    logger.debug("cost of building: {} {} {}", cost.metal.toString(), cost.crystal.toString(), cost.deuter.toString());

    auto storage = planet.getStorage();

    logger.debug("having: {} {} {}", storage.metal.toString(), storage.crystal.toString(), storage.deuter.toString());

    if(hasEnoughToPay(cost, storage))
    {
        logger.debug("Enough to pay");
        if(areRequirementsSatisfied(building, planet.getBuildings(), Researchs{}))
        {

        storage.metal = storage.metal - cost.metal;
        storage.crystal = storage.crystal - cost.crystal;
        storage.deuter = storage.deuter - cost.deuter;
        planet.setNewStorage(storage);

        BuildingQueue queue{
                .building = building,
                .finishAt = timestamp + Duration{timeToBuild(cost)}
        };
        planet.queueBuilding(queue);
        return {};
        }
        logger.warn("Not satisfied requirements");
    }
    logger.warn("Not enough to pay");
    return {};
}

Duration BuildRequestHandler::timeToBuild(const Materials& cost) const
{
    logger.debug("{} + {} = {}", cost.metal.toString(), cost.crystal.toString(), (cost.metal + cost.crystal).toString());
    double costF = std::stod((cost.metal + cost.crystal).toString());

    logger.debug("costF: {}", costF);

    int robotics = planet.getBuildingLevel(cnst::roboticsFactory);
    int nanits = planet.getBuildingLevel(cnst::naniteFactory);

    auto resp = (int)std::max(3600 * costF / (2500 * (1 + robotics) * std::pow(2, nanits) * configuration.buildingSpeed), 1.0);
    logger.debug("calculated time to build: {}", resp);
    return Duration{resp};
}
