#pragma once

#include <boost/hana/define_struct.hpp>

struct PlayerId
{
    BOOST_HANA_DEFINE_STRUCT(PlayerId,
                             (int, id));
};