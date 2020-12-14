#pragma once

#include "BigNum.hpp"
#include "Timestamp.hpp"
#include <boost/hana/define_struct.hpp>

struct Storage
{
    BOOST_HANA_DEFINE_STRUCT(Storage,
                             (Timestamp, lastUpdatedAt),
                             (BigNum, metal),
                             (BigNum, crystal),
                             (BigNum, deuter));
};