#pragma once

#include "RndRequest.hpp"
#include "RndResponse.hpp"
#include "RnDTime.hpp"
#include "TimeForwardResponse.hpp"
#include <variant>

struct RnDService
{
    RndResponse handleRequest(const RndRequest& request)
    {
        return std::visit([this](auto& req)->RndResponse{return {.response = this->handle(req)};}, request.request);
    }

    TimeForwardResponse handle(const TimeForwardRequest&);

    ITime& time;
};