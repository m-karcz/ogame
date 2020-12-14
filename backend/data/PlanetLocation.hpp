#pragma once
#include <boost/hana/adapt_struct.hpp>

struct PlanetLocation
{
    int galaxy;
    int solar;
    int position;
};

BOOST_HANA_ADAPT_STRUCT(PlanetLocation, galaxy, solar, position);