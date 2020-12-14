//
// Created by mkarcz on 03.12.2020.
//
#include "RnDService.hpp"

bool RnDService::handle(const TimeForwardRequest& req)
{
    time.requestShifting(req);
    return true;
}
