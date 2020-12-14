#pragma once

#include "Buildings.hpp"
#include <boost/hana/define_struct.hpp>

struct BuildingsListRequest
{
    BOOST_HANA_DEFINE_STRUCT(BuildingsListRequest);
};

struct BuildingsListResponse
{
    BOOST_HANA_DEFINE_STRUCT(BuildingsListResponse,
                             (Buildings, buildings));
};
