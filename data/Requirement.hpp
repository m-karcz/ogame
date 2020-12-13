#pragma once

#include "Building.hpp"
#include "Research.hpp"
#include <boost/hana/define_struct.hpp>

struct LeveledBuilding
{
    BOOST_HANA_DEFINE_STRUCT(LeveledBuilding,
                             (int, level),
                             (Building, building));
};

struct LeveledResearch
{
    BOOST_HANA_DEFINE_STRUCT(LeveledResearch,
                             (int, level),
                             (Research, research));
};

struct Requirement
{
    BOOST_HANA_DEFINE_STRUCT(Requirement,
                             (std::vector<LeveledBuilding>, buildings),
                             (std::vector<LeveledResearch>, researchs));
};