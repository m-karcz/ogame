#pragma once

#include "ResponseVariant.hpp"
#include "RequestVariant.hpp"
#include "RnDRequestVariant.hpp"
#include "IStorageDb.hpp"
#include "ITime.hpp"
#include "GeneralRequest.hpp"
#include "OnPlanetRequest.hpp"


struct Service
{
    Service(IStorageDb& storageDb, ITime& time) : storageDb{storageDb}, time{time}
    {}
    std::vector<ResponseVariant> handleRequests(const std::vector<RequestVariant>& req);
    ResponseVariant handleRequest(const RequestVariant&);
    OnPlanetResponse onPlanetRequest(const OnPlanetRequest&);
    GeneralResponse handleRequest(const GeneralRequest&);
private:
    StorageResponse handle(IPlayerHandle&, IPlanetHandle&, const StorageRequest&);
    LoginResponse handle(const LoginRequest&);
    RegisterResponse handle(const RegisterRequest&);
    BuildResponse handle(IPlayerHandle&, IPlanetHandle&, const BuildRequest&);
    BuildingsListResponse handle(IPlayerHandle&, IPlanetHandle&, const BuildingsListRequest&);
    BuildingQueueResponse handle(IPlayerHandle&, IPlanetHandle&, const BuildingQueueRequest&);
    void evaluateTimeline(IPlayerHandle& player, IPlanetHandle& planet);
    IStorageDb& storageDb;
    ITime& time;
};