#pragma once

#include <boost/hana/define_struct.hpp>

struct UserCredentials
{
    BOOST_HANA_DEFINE_STRUCT(UserCredentials,
                             (std::string, login),
                             (std::string, passcode));
};