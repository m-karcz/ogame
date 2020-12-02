#pragma once

#include <boost/hana/define_struct.hpp>


struct Buildings
{
    BOOST_HANA_DEFINE_STRUCT(Buildings,
                             (int, metalMine),
                             (int, crystalMine),
                             (int, deuterExtractor),
                             (int, solarGenerator),
                             (int, fusionGenerator),
                             (int, robots),
                             (int, nanits),
                             (int, shipyard),
                             (int, metalStorage),
                             (int, crystalStorage),
                             (int, deuterStorage),
                             (int, researchLab),
                             (int, deposit),
                             (int, silos),
                             (int, terraformer));
};

