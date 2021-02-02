#pragma once

#include "RnDRequest.hpp"
#include "RnDResponse.hpp"
#include "RnDTime.hpp"
#include "TimeForwardResponse.hpp"
#include <variant>

struct RnDService
{
    RnDResponse handleRequest(const RnDRequest& request)
    {
        return std::visit([this](auto& req)->RnDResponse{return {.response = this->handle(req)};}, request.request);
    }

    TimeForwardResponse handle(const TimeForwardRequest&);

    ITime& time;
};