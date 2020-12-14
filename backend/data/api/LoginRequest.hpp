#pragma once
#include <boost/hana/define_struct.hpp>
#include "UserCredentials.hpp"
#include "PlayerId.hpp"
#include "PlanetLocation.hpp"
#include <vector>
#include <optional>

struct LoginRequest
{
    BOOST_HANA_DEFINE_STRUCT(LoginRequest,
                             (UserCredentials, credentials));
};

struct LoginResponse
{
    BOOST_HANA_DEFINE_STRUCT(LoginResponse,
                             (std::optional<PlayerId>, playerId),
                             (std::vector<PlanetLocation>, planets));
};
