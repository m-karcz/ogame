#pragma once

#include <boost/hana/define_struct.hpp>
#include "UserCredentials.hpp"
#include <string>

struct RegisterRequest
{
    BOOST_HANA_DEFINE_STRUCT(RegisterRequest,
                             (UserCredentials, credentials));
};

struct RegisterResponse
{
    BOOST_HANA_DEFINE_STRUCT(RegisterResponse,
                             (std::string, status));
};
