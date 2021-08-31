#pragma once

#include "Event.hpp"
#include "WorkStealerReady.hpp"
#include "AuthenticatedOnPlanetRequest.hpp"
#include "OnPlanetResponseNew.hpp"
#include "LogicFsm.hpp"

struct LogicApp
{
    LogicApp(IAsyncWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew>& workStealer,
             IAsyncRequester<LockRequestNew,LockResponseNew>& resOwnConn,
             IStorageDb& storageDb,
             const Configuration& configuration);

    LogicService fsm;
};