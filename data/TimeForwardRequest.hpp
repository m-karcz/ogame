#pragma once

#include <boost/hana/define_struct.hpp>

struct TimeForwardRequest
{
    BOOST_HANA_DEFINE_STRUCT(TimeForwardRequest,
                             (Duration, duration));
};

struct TimeForwardResponse
{
    BOOST_HANA_DEFINE_STRUCT(TimeForwardResponse,
                             (std::string, status));
};
