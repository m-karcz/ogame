#pragma once

#include "api/RnDRequestVariant.hpp"
#include "RnDTime.hpp"
#include <variant>

struct RnDService
{
    bool handleRequest(const RnDRequestVariant& request)
    {
        return std::visit([this](auto& req)->bool{return this->handle(req);}, request);
    }

    bool handle(const TimeForwardRequest&);

    RnDTime& time;
};