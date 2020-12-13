#include "Service.hpp"
#include "api/LoginRequest.hpp"
#include "api/RegisterRequest.hpp"
#include "api/StorageRequest.hpp"
#include "api/TimeForwardRequest.hpp"
#include "api/BuildRequest.hpp"
#include "PlayerId.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "ProductionCalculator.hpp"
#include "BuildingsData.hpp"
#include "BuildingQueue.hpp"
#include "Logger.hpp"
#include "handlers/BuildRequestHandler.hpp"
#include "handlers/BuildingsListRequestHandler.hpp"
#include "handlers/BuildingQueueRequestHandler.hpp"
#include "handlers/StorageRequestHandler.hpp"
#include "api/OnPlanetRequest.hpp"
#include <boost/hana/map.hpp>
#include <boost/hana/at_key.hpp>

namespace hana = boost::hana;

namespace
{
template<typename Key, typename Value>
constexpr auto kvPair = hana::make_pair(hana::type_c<Key>, hana::type_c<Value>);
constexpr auto actionHandlers = hana::make_map(kvPair<BuildRequest,BuildRequestHandler>);
constexpr auto queryHandlers = hana::make_map(kvPair<BuildingsListRequest,BuildingsListRequestHandler>,
                                              kvPair<BuildingQueueRequest,BuildingQueueRequestHandler>,
                                              kvPair<StorageRequest      ,StorageRequestHandler>);
template<typename Action>
using ActionHandlerType = typename decltype(+actionHandlers[hana::type_c<Action>])::type;
template<typename Query>
using QueryHandlerType = typename decltype(+queryHandlers[hana::type_c<Query>])::type;

void evaluateTimeline(SinglePlanetContext& ctx)
{
    auto timestamp = ctx.timestamp;

    auto& planet = ctx.planet;
    auto& player = ctx.player;

    auto buildingQueue = planet.getBuildingQueue();
    auto buildings = planet.getBuildings();
    auto storage = planet.getStorage();

    ProductionCalculator prodCalc;

    if(buildingQueue and buildingQueue->finishAt < timestamp)
    {
        logger.debug("Building queue to finish at {} now has {}", buildingQueue->finishAt.time_since_epoch().count(), timestamp.time_since_epoch().count());
        auto finishBuildingTimestamp = buildingQueue->finishAt;
        prodCalc.updateStorage(storage, finishBuildingTimestamp, buildings);

        planet.dequeueBuilding(*buildingQueue);
        planet.incrementBuildingLevel(buildingQueue->building);
        buildings = planet.getBuildings();
    }

    prodCalc.updateStorage(storage, timestamp, buildings);
    planet.setNewStorage(storage);
}
}


GeneralResponse Service::handleRequest(const GeneralRequest& request)
{
    return std::visit([this](auto& req)->GeneralResponse{return this->handle(req);}, request);
}

OnPlanetResponse Service::handleSinglePlanetRequest(const OnPlanetRequest& request)
{
    OnPlanetResponse resp;

    auto playerHandle = storageDb.queryPlayer(request.playerId);
    auto planetHandle = playerHandle->getPlanet(request.planet);

    SinglePlanetContext ctx{*playerHandle, *planetHandle, time.getTimestamp()};

    evaluateTimeline(ctx);

    if(request.action)
    {
        std::visit([&](auto&& action){resp.push_back(ActionHandlerType<std::decay_t<decltype(action)>>{ctx}.handleAction(action));}, *request.action);
    }
    for(auto&& query : request.queries)
    {
        std::visit([&](auto&& q){resp.push_back(QueryHandlerType<std::decay_t<decltype(q)>>{ctx}.handleQuery(q));}, query);
    }
    return resp;
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
    auto res = storageDb.registerPlayer(req.credentials);
    if(res)
    {
        PlanetLocation planetLoc = {.galaxy = 1, .solar = 1, .position = 7};
        auto player = storageDb.queryPlayer(req.credentials);
        player->createPlanet(planetLoc, time.getTimestamp());
    }
    return RegisterResponse{.status = res ? "ok" : "not ok"};
}


