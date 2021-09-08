#include "LogicFsm.hpp"
#include "Fsm.hpp"
#include "SinglePlanetContext.hpp"
#include "IStorageDb.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "LockRequestNew.hpp"
#include "LockResponseNew.hpp"
#include "procedures/EvaluateTimelineProcedure.hpp"
#include "handlers/BuildRequestHandler.hpp"
#include "BuildRequest.hpp"
#include "BuildingQueue.hpp"
#include "BuildingQueueRequest.hpp"
#include "BuildingQueueResponse.hpp"
#include "BuildResponse.hpp"
#include "AuthenticatedOnPlanetRequest.hpp"
#include "Overloaded.hpp"
#include <iostream>
#include "Logger.hpp"
#include "BuildingQueueEntry.hpp"

struct LogicCtx
{};

namespace
{
struct WaitingForRequest;
struct NewAccountChecking;
struct ProcessingOnPlanet;

using MainRequest = Request<AuthenticatedOnPlanetRequest, OnPlanetResponseNew>;

struct Continue
{};

struct WaitingForRequest
{
    MARK_UNEXPECTED(Continue);
    MARK_UNEXPECTED(Event<LockResponseNew>);
    Transition<NewAccountChecking> process(LogicService::Impl* self, const MainRequest& request);
};

struct NewAccountChecking
{
    PossibleTransition<ProcessingOnPlanet> process(LogicService::Impl* self, const Continue&);
    Transition<ProcessingOnPlanet> process(LogicService::Impl* self, const Event<LockResponseNew>&);
    MARK_UNEXPECTED(MainRequest);
};

struct ProcessingOnPlanet
{
    NoTransition process(LogicService::Impl* self, const Continue&);
    PossibleTransition<WaitingForRequest> process(LogicService::Impl* self, const Event<LockResponseNew>& lockResponse);
    MARK_UNEXPECTED(MainRequest);
};

using States = TypeList<WaitingForRequest,
                        NewAccountChecking,
                        ProcessingOnPlanet>;

using Events = TypeList<MainRequest,
                        Event<LockResponseNew>,
                        Continue>;
}



struct LogicService::Impl : Fsm<Events,
                                States,
                                LogicService::Impl*>
{
    Impl(IStorageDb& db, const Configuration& configuration, IAsyncRequester<LockRequestNew,LockResponseNew>& resOwnConn) : Fsm<Events, States, LogicService::Impl*>(this),
                                        db{db},
                                        configuration{configuration},
                                        resourceOwnerConn{resOwnConn}
    {}

    std::vector<PlayerId> getNeededPlayerIds()
    {
        return {playerHandle->getId()};
    }
    void storeRequest(const MainRequest& request)
    {
        onPlanetRequest = std::make_unique<MainRequest>(request);
    }

    void sendRequestForLockingPlayers(const std::vector<PlayerId>& playerIds)
    {
        resourceOwnerConn.sendRequest(LockRequestNew{});
    }

    void finishEvaluatingContext(const LockResponseNew& response)
    {
        auto x = SinglePlanetContext{*playerHandle,
                                  *planetHandle,
                                  response.timestamp,
                                  configuration};
        ctx = std::make_unique<SinglePlanetContext>(x);
    }

    std::unique_ptr<SinglePlanetContext> ctx;
    std::unique_ptr<MainRequest> onPlanetRequest;
    std::shared_ptr<IPlayerHandle> playerHandle;
    std::shared_ptr<IPlanetHandle> planetHandle;
    IStorageDb& db;
    const Configuration& configuration;
    IAsyncRequester<LockRequestNew, LockResponseNew>& resourceOwnerConn;
};


Transition<NewAccountChecking> WaitingForRequest::process(LogicService::Impl* self, const MainRequest& request)
{
    logger.debug("started processing request in LogicFsm");
    self->storeRequest(request);
    self->processEvent(Continue{});
    return Transition<NewAccountChecking>{};
}

PossibleTransition<ProcessingOnPlanet> NewAccountChecking::process(LogicService::Impl* self, const Continue&)
{
    logger.debug("checking if its new account");
    self->playerHandle = self->db.queryPlayer((*self->onPlanetRequest)->playerId);
    auto planets = self->playerHandle->getPlanetList();
    if(planets.empty())
    {
        self->resourceOwnerConn.sendRequest(LockRequestNew{
            .instanceId = 0,
            .data = LockPlanetCreation{}
        });
        return NoTransition{};
    }
    else
    {
        self->processEvent(Continue{});
        return Transition<ProcessingOnPlanet>{};
    }
}

Transition<ProcessingOnPlanet> NewAccountChecking::process(LogicService::Impl* self, const Event<LockResponseNew>& lockResponse)
{
    logger.debug("received lock for creating new planet");
    auto id = self->playerHandle->getId().id;
    self->playerHandle->createPlanet(PlanetLocation{.galaxy = (id / (499 * 6)) % 9 + 1,
                                                .solar = (id / 6) % 499 + 1,
                                                .position = id % 6 + 4},
                                     lockResponse->timestamp);

    self->processEvent(Continue{});
    return Transition<ProcessingOnPlanet>{};
}

NoTransition ProcessingOnPlanet::process(LogicService::Impl* self, const Continue&)
{
    auto& chosenPlanet = (*self->onPlanetRequest)->request.planet;
    self->planetHandle = self->playerHandle->getPlanet(chosenPlanet ? *chosenPlanet : self->playerHandle->getPlanetList()[0]);
    
    logger.debug("sent lock request for processing on planet");
    self->resourceOwnerConn.sendRequest(LockRequestNew{
        .instanceId = 0,
        .data = LockPlayersNew{
            .players = { self->playerHandle->getId() }
        }
    });
    return NoTransition{};
}

static auto getBuildingQueue(SinglePlanetContext& ctx)
{
    std::optional<BuildingQueueEntry> entry{};
    if(auto queue = ctx.planet.getBuildingQueue())
    {
        entry = BuildingQueueEntry{
                .building = queue->building,
                .timeToFinish = Duration{queue->finishAt - ctx.timestamp}
        };
    }
    return entry;
}

PossibleTransition<WaitingForRequest> ProcessingOnPlanet::process(LogicService::Impl* self, const Event<LockResponseNew>& lockResponse)
{
    logger.debug("received lock for on planet");
    self->finishEvaluatingContext(*lockResponse);
    evaluateTimelineProcedure(*self->ctx);
    if((*self->onPlanetRequest)->request.action)
    {
        std::visit(overloaded{
            [self](const BuildRequest& req){ BuildRequestHandler{*self->ctx}.handleAction(req);}
        }, *(*self->onPlanetRequest)->request.action);
    }
    self->onPlanetRequest->respond(OnPlanetResponseNew{
        .response = {
            .storage = self->planetHandle->getStorage(),
            .buildings = self->planetHandle->getBuildings(),
            .buildingQueue = getBuildingQueue(*self->ctx),
            .productionInformation = {
                .percentages = self->planetHandle->getProductionPercentages(),
                .production = self->planetHandle->getCachedProduction()
            },
            .planetList = self->playerHandle->getPlanetList()
        }
    });
    return Transition<WaitingForRequest>{};
}


LogicService::LogicService(IStorageDb& storageDb, const Configuration& configuration, IAsyncRequester<LockRequestNew,LockResponseNew>& resOwnConn)
{
    impl = std::make_unique<Impl>(storageDb, configuration, resOwnConn);
}

void LogicService::process(MainRequest request)
{
    impl->processEvent(request);
}

void LogicService::process(Event<LockResponseNew> request)
{
    impl->processEvent(request);
}

LogicService::~LogicService() = default;
