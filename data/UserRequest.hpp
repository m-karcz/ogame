#pragma once

#include "PlayerId.hpp"
#include "RequestVariant.hpp"

struct UserRequest
{
    PlayerId playerId;
    std::vector<RequestVariant> request;
};