//
// Created by mkarcz on 03.12.2020.
//
#include "RnDService.hpp"

TimeForwardResponse RnDService::handle(const TimeForwardRequest& req)
{
    time.shiftTimeBy(req.duration);
    return {.status = Ok{} };
}
