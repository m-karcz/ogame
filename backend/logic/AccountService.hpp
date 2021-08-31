#pragma once

#include "LoginRequest.hpp"
#include "RegisterRequest.hpp"
#include "InternalLoginResponse.hpp"
#include "RegisterResponseNew.hpp"
#include "Event.hpp"
#include "IStorageDb.hpp"


struct AccountService
{
    AccountService(IStorageDb& storageDb);
    void process(Request<LoginRequest, InternalLoginResponse>);
    void process(Request<RegisterRequest, RegisterResponseNew>);
    IStorageDb& storageDb;
};