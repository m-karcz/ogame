#pragma once

#include "PlayerId.hpp"
#include "api/RequestVariant.hpp"

struct UserRequest
{
    PlayerId playerId;
    std::vector<RequestVariant> request;
};