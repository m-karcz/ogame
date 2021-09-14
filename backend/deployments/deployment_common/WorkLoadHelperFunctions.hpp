#pragma once

#include "WorkSharedPlanetRequest.hpp"
#include <variant>

bool isWorkerReady(const WorkSharedPlanetRequest& req)
{
    return std::holds_alternative<WorkStealerReady>(req.data);
}

const OnPlanetResponseNew& getWorkResponse(const WorkSharedPlanetRequest& req)
{
    return std::get<OnPlanetResponseNew>(req.data);
}

WorkSharedPlanetRequest wrapWorkerReady(const WorkStealerReady& ready)
{
    return {
        ready
    };
}

WorkSharedPlanetRequest wrapWorkResponse(const OnPlanetResponseNew& resp)
{
    return {
        resp
    };
}