#include "Service.hpp"
#include "LoginRequest.hpp"
#include "RegisterRequest.hpp"
#include "StorageRequest.hpp"
#include "BuildRequest.hpp"
#include "PlayerId.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "ProductionCalculator.hpp"
#include "BuildingQueue.hpp"
#include "Logger.hpp"
#include "handlers/BuildRequestHandler.hpp"
#include "handlers/BuildingsListRequestHandler.hpp"
#include "handlers/BuildingQueueRequestHandler.hpp"
#include "handlers/StorageRequestHandler.hpp"
#include "handlers/ProductionPercentagesRequestHandler.hpp"
#include "handlers/ProductionInformationRequestHandler.hpp"
#include "OnPlanetRequest.hpp"
#include "OnPlanetResponse.hpp"
#include <boost/hana/map.hpp>
#include <boost/hana/at_key.hpp>
#include "procedures/ProductionRecalculationProcedure.hpp"
#include "procedures/ProductionStorageUpdateProcedure.hpp"

namespace hana = boost::hana;

namespace
{
template<typename Key, typename Value>
constexpr auto kvPair = hana::make_pair(hana::type_c<Key>, hana::type_c<Value>);
constexpr auto actionHandlers = hana::make_map(kvPair<BuildRequest,BuildRequestHandler>);
constexpr auto queryHandlers = hana::make_map(kvPair<BuildingsListRequest,BuildingsListRequestHandler>,
                                              kvPair<BuildingQueueRequest,BuildingQueueRequestHandler>,
                                              kvPair<StorageRequest      ,StorageRequestHandler>,
                                              kvPair<ProductionPercentagesRequest,ProductionPercentagesRequestHandler>,
                                              kvPair<ProductionInformationRequest,ProductionInformationRequestHandler>);
template<typename Action>
using ActionHandlerType = typename decltype(+actionHandlers[hana::type_c<Action>])::type;
template<typename Query>
using QueryHandlerType = typename decltype(+queryHandlers[hana::type_c<Query>])::type;

void evaluateTimeline(SinglePlanetContext& ctx)
{
    auto timestamp = ctx.timestamp;

    auto& planet = ctx.planet;

    auto buildingQueue = planet.getBuildingQueue();

    logger.debug("everything queried");

    if(buildingQueue and buildingQueue->finishAt < timestamp)
    {
        logger.debug("Building queue to finish at {} now has {}", buildingQueue->finishAt.time_since_epoch().count(), timestamp.time_since_epoch().count());
        auto finishBuildingTimestamp = buildingQueue->finishAt;
        productionStorageUpdateProcedure(ctx, finishBuildingTimestamp);

        planet.dequeueBuilding(*buildingQueue);
        planet.incrementBuildingLevel(buildingQueue->building);
        productionRecalculationProcedure(ctx);
    }

    productionStorageUpdateProcedure(ctx);
}
}


GeneralResponse Service::handleRequest(const GeneralRequest& request)
{
    return std::visit([this](auto& req)->GeneralResponse{return {.response = this->handle(req)};}, request.request);
}

OnPlanetResponse Service::handleSinglePlanetRequest(const OnPlanetRequest& request)
{
    logger.debug("processing single planet request");
    OnPlanetResponse resp;

    auto playerHandle = storageDb.queryPlayer(request.playerId);
    auto planetHandle = playerHandle->getPlanet(request.planet);

    SinglePlanetContext ctx{*playerHandle, *planetHandle, time.getTimestamp(), configuration};

    evaluateTimeline(ctx);

    if(request.action)
    {
        std::visit([&](auto&& action){resp.response.push_back(ActionHandlerType<std::decay_t<decltype(action)>>{ctx}.handleAction(action));}, *request.action);
    }
    for(auto&& query : request.queries)
    {
        std::visit([&](auto&& q){logger.debug("Handling query"); logger.debug(__PRETTY_FUNCTION__);
            resp.response.push_back(QueryHandlerType<std::decay_t<decltype(q)>>{ctx}.handleQuery(q));}, query);
    }
    logger.debug("handled all queries");
    return resp;
}

OnPlanetResponseNew Service::handleSinglePlanetRequest(const OnPlanetRequestNew& request)
{
    OnPlanetQueriesResponse resp;
    auto playerHandle = storageDb.queryPlayer(request.playerId);
    auto planetHandle = playerHandle->getPlanet(request.planet);

    SinglePlanetContext ctx{*playerHandle, *planetHandle, time.getTimestamp(), configuration};

    evaluateTimeline(ctx);
    if(request.action)
    {
        std::visit([&](auto&& action){resp.response.push_back(ActionHandlerType<std::decay_t<decltype(action)>>{ctx}.handleAction(action));}, *request.action);
    }

    const auto handle = [&](auto handler, auto getter){
        if(auto& req = getter(request))
        {
            getter(resp) = handler.handleQuery(*req);
        }
    };

    handle(StorageRequestHandler{ctx}, [](auto& req)->auto&{return req.storage;});
    handle(BuildingsListRequestHandler{ctx}, [](auto& req)->auto&{return req.buildings;});
    handle(BuildingQueueRequestHandler{ctx}, [](auto& req)->auto&{return req.buildingQueue;});
    handle(ProductionInformationRequestHandler{ctx}, [](auto& req)->auto&{return req.productionInformation;});
    handle(ProductionPercentagesRequestHandler{ctx}, [](auto& req)->auto&{return req.productionPercentages;});

    return {resp};

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
        auto player = storageDb.queryPlayer(req.credentials);
        PlanetLocation planetLoc = {.galaxy = 1, .solar = player->getId().id, .position = 7};
        player->createPlanet(planetLoc, time.getTimestamp());
    }
    return RegisterResponse{.status = Ok{}/*res ? "ok" : "not ok"*/};
}


