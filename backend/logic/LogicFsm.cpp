#include "LogicFsm.hpp"
#include "Fsm.hpp"
#include "SinglePlanetContext.hpp"
#include "IStorageDb.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "LockRequestNew.hpp"
#include "LockResponseNew.hpp"
#include "procedures/EvaluateTimelineProcedure.hpp"
#include <iostream>

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

/*struct WaitingForLock
{
    MARK_UNEXPECTED(MainRequest);
    Transition<WaitingForRequest> process(LogicService::Impl* self, const Event<LockResponseNew>& lockResponse);
};*/

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
    void storeRequestAndHalfEvaluateContext(const MainRequest& request)
    {
        onPlanetRequest = std::make_unique<MainRequest>(request);
        /*playerHandle = db.queryPlayer(request->playerId);
        auto planets = playerHandle->getPlanetList();
        if(planets.empty())
        {
            auto id = playerHandle->getId().id;
            playerHandle->createPlanet(PlanetLocation{.galaxy = (id / (499 * 6)) % 9 + 1,
                                                      .solar = (id / 6) % 499 + 1,
                                                      .position = id % 6 + 4},
                                        Timestamp{Duration{0}});
            planets = playerHandle->getPlanetList();
        }
        planetHandle = playerHandle->getPlanet(request->request.planet ? *request->request.planet : planets[0]);*/
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
    std::cout << "processing request" << std::endl;
    self->storeRequestAndHalfEvaluateContext(request);
    /*auto&& neededPlayerIds = self->getNeededPlayerIds();
    self->sendRequestForLockingPlayers(neededPlayerIds);
    return Transition<WaitingForLock>();*/
    self->processEvent(Continue{});
    return Transition<NewAccountChecking>{};
}

/*Transition<WaitingForRequest> WaitingForLock::process(LogicService::Impl* self, const Event<LockResponseNew>& lockResponse)
{
}*/

PossibleTransition<ProcessingOnPlanet> NewAccountChecking::process(LogicService::Impl* self, const Continue&)
{
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
    
    self->resourceOwnerConn.sendRequest(LockRequestNew{
        .instanceId = 0,
        .data = LockPlayersNew{
            .players = { self->playerHandle->getId() }
        }
    });
    return NoTransition{};
}
PossibleTransition<WaitingForRequest> ProcessingOnPlanet::process(LogicService::Impl* self, const Event<LockResponseNew>& lockResponse)
{
    std::cout << "received lock" << std::endl;
    self->finishEvaluatingContext(*lockResponse);
    evaluateTimelineProcedure(*self->ctx);
    self->onPlanetRequest->respond(OnPlanetResponseNew{
        .response = {
            .storage = self->planetHandle->getStorage(),
            .buildings = self->planetHandle->getBuildings(),
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
    std::cout << "processing main request" << std::endl;
    impl->processEvent(request);
}

void LogicService::process(Event<LockResponseNew> request)
{
    impl->processEvent(request);
}

LogicService::~LogicService() = default;
