#pragma once

#include "Event.hpp"
#include "AuthenticatedOnPlanetRequest.hpp"
#include "OnPlanetResponseNew.hpp"
#include <memory>
#include "LockResponse.hpp"

struct IStorageDb;
struct Configuration;

struct LockRequestNew;
struct LockResponseNew;

struct LogicService
{
    struct Impl;
    LogicService(IStorageDb&, const Configuration&, IAsyncRequester<LockRequestNew,LockResponseNew>& resOwnConn);
    ~LogicService();
    void process(Request<AuthenticatedOnPlanetRequest, OnPlanetResponseNew>);
    void process(Event<LockResponseNew>);
    std::unique_ptr<Impl> impl;
};

