#include "BuildRequest.hpp"
#include "BuildResponse.hpp"
#include "BuildRequestHandler.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"
#include "Building.hpp"
#include "BuildingsData.hpp"
#include "BuildingQueue.hpp"


namespace
{
unsigned calculateTimeToBuild(const Materials& cost)
{
    logger.debug("{} + {} = {}", cost.metal.toString(), cost.crystal.toString(), (cost.metal + cost.crystal).toString());
    double costF = std::stod((cost.metal + cost.crystal).toString());

    logger.debug("costF: {}", costF);

    int robotics = 0;
    int nanits = 0;

    auto resp = 3600 * costF / (2500 * (1 + robotics) * std::pow(2, nanits));
    logger.debug("calculated time to build: {}", resp);
    return resp;
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

    if(hasEnoughToPay(cost, storage))
    {
        logger.debug("Enough to pay");
        storage.metal = storage.metal - cost.metal;
        storage.crystal = storage.crystal - cost.crystal;
        storage.deuter = storage.deuter - cost.deuter;
        planet.setNewStorage(storage);

        BuildingQueue queue{
                .building = building,
                .finishAt = timestamp + Duration{calculateTimeToBuild(cost)}
        };
        planet.queueBuilding(queue);
        return {};
    }
    logger.warn("Not enough to pay");
    return {};
}

