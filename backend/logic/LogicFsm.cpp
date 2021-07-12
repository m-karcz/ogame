#include "LogicFsm.hpp"
#include "Fsm.hpp"
#include "SinglePlanetContext.hpp"
#include "IStorageDb.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "procedures/EvaluateTimelineProcedure.hpp"

struct LogicCtx
{};

namespace
{
struct WaitingForRequest;
struct WaitingForLock;

struct WaitingForRequest
{
    MARK_UNEXPECTED(Request<LockResponse>);
    Transition<WaitingForLock> process(LogicService::Impl* self, const Request<OnPlanetRequestNew>& request);
};
struct WaitingForLock
{
    MARK_UNEXPECTED(Request<OnPlanetRequestNew>);
    Transition<WaitingForRequest> process(LogicService::Impl* self, const Request<LockResponse>& lockResponse);
};

using States = TypeList<WaitingForRequest,
                        WaitingForLock>;

using Events = TypeList<Request<OnPlanetRequestNew>,
                        Request<LockResponse>>;
}



struct LogicService::Impl : Fsm<Events,
                                States,
                                LogicService::Impl*>
{
    Impl(IStorageDb& db, const Configuration& configuration) : Fsm<Events, States, LogicService::Impl*>(this),
                                        db{db},
                                        configuration{configuration}
    {}

    std::vector<PlayerId> getNeededPlayerIds()
    {
        return {playerHandle->getId()};
    }
    void storeRequestAndHalfEvaluateContext(const Request<OnPlanetRequestNew>& request)
    {
        onPlanetRequest = std::make_unique<Request<OnPlanetRequestNew>>(request);
        playerHandle = db.queryPlayer(request->playerId);
        planetHandle = playerHandle->getPlanet(request->planet);
    }

    void sendRequestForLockingPlayers(const std::vector<PlayerId>& playerIds)
    {
        
    }

    void finishEvaluatingContext(const LockResponse& response)
    {
        auto x = SinglePlanetContext{*playerHandle,
                                  *planetHandle,
                                  response.timestamp,
                                  configuration};
        ctx = std::make_unique<SinglePlanetContext>(x);
    }

    std::unique_ptr<SinglePlanetContext> ctx;
    std::unique_ptr<Request<OnPlanetRequestNew>> onPlanetRequest;
    std::shared_ptr<IPlayerHandle> playerHandle;
    std::shared_ptr<IPlanetHandle> planetHandle;
    IStorageDb& db;
    const Configuration& configuration;
};


Transition<WaitingForLock> WaitingForRequest::process(LogicService::Impl* self, const Request<OnPlanetRequestNew>& request)
{
    self->storeRequestAndHalfEvaluateContext(request);
    auto&& neededPlayerIds = self->getNeededPlayerIds();
    self->sendRequestForLockingPlayers(neededPlayerIds);
    return Transition<WaitingForLock>();
}

Transition<WaitingForRequest> WaitingForLock::process(LogicService::Impl* self, const Request<LockResponse>& lockResponse)
{
    self->finishEvaluatingContext(*lockResponse);
    evaluateTimelineProcedure(*self->ctx);
    return Transition<WaitingForRequest>{};
}


LogicService::LogicService(IStorageDb& storageDb, const Configuration& configuration)
{
    impl = std::make_unique<Impl>(storageDb, configuration);
}

void LogicService::process(Request<OnPlanetRequestNew> request)
{
    impl->processEvent(request);
}

void LogicService::process(Request<LockResponse> request)
{
    impl->processEvent(request);
}

LogicService::~LogicService() = default;
