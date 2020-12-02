#include "Service.hpp"
#include "LoginRequest.hpp"
#include "RegisterRequest.hpp"
#include "StorageRequest.hpp"
#include "TimeForwardRequest.hpp"
#include "BuildRequest.hpp"
#include "PlayerId.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "ProductionCalculator.hpp"
#include "BuildingsData.hpp"
#include "RnDTime.hpp"
#include <iostream>
#include "Logger.hpp"

ResponseVariant Service::handleRequest(const RequestVariant& request)
{
    return {};//std::visit([this](auto& req)->ResponseVariant{return this->handle(req);}, request);
}

GeneralResponse Service::handleRequest(const GeneralRequest& request)
{
    return std::visit([this](auto& req)->GeneralResponse{return this->handle(req);}, request);
}

OnPlanetResponse Service::onPlanetRequest(const OnPlanetRequest& request)
{
    OnPlanetResponse ret;
    auto playerHandle = storageDb.queryPlayer(request.playerId);
    auto planetHandle = playerHandle->getPlanet(request.planet);
    for(auto i : request.requests)
    {
        std::visit([&](auto& req){ret.push_back(this->handle(*playerHandle, *planetHandle, req));}, i);
    }
    return ret;
}

std::vector<ResponseVariant> Service::handleRequests(const std::vector<RequestVariant>& req)
{
    std::vector<ResponseVariant> ret;
    for(auto i : req)
    {
        ret.push_back(handleRequest(i));
    }
    return ret;
}

bool Service::handleRequest(const RnDRequestVariant& request)
{
    return std::visit([this](auto& req)->bool{return this->handle(req);}, request);
}

StorageResponse Service::handle(IPlayerHandle& player, IPlanetHandle& planet, const StorageRequest&)
{
    return {};
}

namespace
{
unsigned calculateTimeToBuild(const Materials& cost)
{
    double costF = std::stod((cost.metal + cost.crystal).toString());

    int robotics = 0;
    int nanits = 0;

    return 3600 * costF / (2500 * (1 + robotics) * std::pow(2, nanits));
}
}

BuildResponse Service::handle(IPlayerHandle& player, IPlanetHandle& planet, const BuildRequest& req)
{
    logger.debug(__PRETTY_FUNCTION__);
    Building building{req.buildingName};

    logger.debug("Got building to built: {}", building.fieldName);
    int level = planet.getBuildingLevel(building);

    auto cost = getBuildingCost(building, level);

    auto storage = planet.getStorage();

    if(hasEnoughToPay(cost, storage))
    {
        logger.debug("Enough to pay");
      /*  auto& mut = *storage->mut();
        mut.metal = mut.metal - cost.metal;
        mut.crystal = mut.crystal - cost.crystal;
        mut.deuter = mut.deuter - cost.deuter;*/
        storage.metal = storage.metal - cost.metal;
        storage.crystal = storage.crystal - cost.crystal;
        storage.deuter = storage.deuter - cost.deuter;
        planet.setNewStorage(storage);



        BuildingQueue queue{
            .building = building,
            .finishAt = time.getTimestamp() + Duration{calculateTimeToBuild(cost)}
        };
        planet.queueBuilding(queue);
        return {};
    }
    logger.debug("Not enough to pay");
    return {};
}

BuildingsListResponse Service::handle(IPlayerHandle& player, IPlanetHandle& planet, const BuildingsListRequest&)
{
    std::cout << "handling buildings list" << std::endl;
    evaluateTimeline(player, planet);
    std::cout << "evaluated timeline" << std::endl;
    return {planet.getBuildings()};
}

bool Service::handle(const TimeForwardRequest& req)
{
    //time.setTimeTo(time.getTimestamp() + req.duration);
    dynamic_cast<RnDTime&>(time).requestShifting(req);
    return true;
}

LoginResponse Service::handle(const LoginRequest& req)
{
    auto res = storageDb.queryPlayer(req.credentials);
    if(res)
    {
        auto planets = res->getPlanetList();
        return LoginResponse{.playerId = res->getId(), .planets = planets};
    }
    return LoginResponse{.playerId = std::nullopt, .planets = {}};
}

RegisterResponse Service::handle(const RegisterRequest& req)
{
    std::cout << "register trial" << std::endl;
    auto res = storageDb.registerPlayer(req.credentials);
    if(res)
    {
        std::cout << "registered" << std::endl;
        PlanetLocation planetLoc = {.galaxy = 1, .solar = 1, .position = 7};
        auto player = storageDb.queryPlayer(req.credentials);
        std::cout << "queried player: " << static_cast<bool>(player) <<  std::endl;
        player->createPlanet(planetLoc, time.getTimestamp());
    }
    return RegisterResponse{.status = res ? "ok" : "not ok"};
}

void Service::evaluateTimeline(IPlayerHandle& player, IPlanetHandle& planet)
{
    auto timestamp = time.getTimestamp();

    auto buildingQueue = planet.getBuildingQueue();
    logger.debug("Got buildingsQueue");
    auto buildings = planet.getBuildings();
    logger.debug("Got buildings");
    auto storage = planet.getStorage();
    logger.debug("Got storage");

    std::cout << "after getters" << std::endl;

    ProductionCalculator prodCalc;


    if(buildingQueue)
    {
        auto finishBuildingTimestamp = buildingQueue->finishAt;
        prodCalc.updateStorage(storage, finishBuildingTimestamp, buildings);

        planet.dequeueBuilding(*buildingQueue);
        planet.incrementBuildingLevel(buildingQueue->building);
        buildings = planet.getBuildings();
    }



    prodCalc.updateStorage(storage, timestamp, buildings);
    planet.setNewStorage(storage);

}

