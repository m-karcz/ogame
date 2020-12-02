#pragma once

#include "UserCredentials.hpp"

namespace sqlite
{

struct UserCredentialsWrapper : UserCredentials
{
    using Base = UserCredentials;
    BOOST_HANA_DEFINE_STRUCT(UserCredentialsWrapper,
                             (int, id));

};

template<> auto columnOptions<&UserCredentialsWrapper::id> = std::make_tuple(sqlite_orm::autoincrement(), sqlite_orm::primary_key());
template<> auto columnOptions<&UserCredentialsWrapper::login> = std::make_tuple(sqlite_orm::unique());

}
