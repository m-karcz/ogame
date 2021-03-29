#pragma once

#include "RndRequest.hpp"
#include "RndResponse.hpp"
#include "RnDTime.hpp"
#include "TimeForwardResponse.hpp"
#include "ClearDatabaseRequest.hpp"
#include "ClearDatabaseResponse.hpp"
#include <variant>

struct IStorageDb;

struct RnDService
{
    RndResponse handleRequest(const RndRequest& request)
    {
        return std::visit([this](auto& req)->RndResponse{return {.response = this->handle(req)};}, request.request);
    }

    TimeForwardResponse handle(const TimeForwardRequest&);
    ClearDatabaseResponse handle(const ClearDatabaseRequest&);

    ITime& time;
    IStorageDb& storageDb;
};