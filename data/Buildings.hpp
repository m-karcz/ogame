#pragma once

#include <boost/hana/define_struct.hpp>


struct Buildings
{
    BOOST_HANA_DEFINE_STRUCT(Buildings,
                             (int, metalMine),
                             (int, crystalMine),
                             (int, deuterExtractor),
                             (int, solarGenerator),
                             (int, fusionReactor),
                             (int, robots),
                             (int, nanites),
                             (int, shipyard),
                             (int, metalStorage),
                             (int, crystalStorage),
                             (int, deuterStorage),
                             (int, researchLab),
                             (int, alianceDepot),
                             (int, missileSilo),
                             (int, terraformer));
};

