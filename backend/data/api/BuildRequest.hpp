#pragma once

#include <boost/hana/define_struct.hpp>
#include <string>
#include "PlanetLocation.hpp"
#include "Status.hpp"

struct BuildRequest
{
    BOOST_HANA_DEFINE_STRUCT(BuildRequest,
                             (std::string, buildingName));
};

struct BuildResponse
{
    BOOST_HANA_DEFINE_STRUCT(BuildResponse,
                             (Status, status));
};