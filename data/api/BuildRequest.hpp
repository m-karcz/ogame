#pragma once

#include <boost/hana/define_struct.hpp>
#include <string>
#include "PlanetLocation.hpp"

struct BuildRequest
{
    BOOST_HANA_DEFINE_STRUCT(BuildRequest,
                             (std::string, buildingName));
};

struct BuildResponse
{
    BOOST_HANA_DEFINE_STRUCT(BuildResponse,
                             (std::string, status));
};