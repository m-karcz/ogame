#pragma once

#include <boost/hana/define_struct.hpp>

struct Researchs
{
    BOOST_HANA_DEFINE_STRUCT(Researchs,
                             (int,espionage),
                             (int,computer),
                             (int,weapons),
                             (int,shielding),
                             (int,armour),
                             (int,energy),
                             (int,hyperspace),
                             (int,combustionDrive),
                             (int,impulseDrive),
                             (int,hyperspaceDrive),
                             (int,laser),
                             (int,ion),
                             (int,plasma),
                             (int,intergalactic),
                             (int,graviton));
};
