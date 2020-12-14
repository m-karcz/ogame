#pragma once

#include "api/ResponseVariant.hpp"
#include "api/RequestVariant.hpp"
#include "api/RnDRequestVariant.hpp"
#include "IStorageDb.hpp"
#include "ITime.hpp"
#include "api/GeneralRequest.hpp"
#include "api/OnPlanetRequest.hpp"



struct Service
{
    Service(IStorageDb& storageDb, ITime& time) : storageDb{storageDb}, time{time}
    {}
    GeneralResponse handleRequest(const GeneralRequest&);
    OnPlanetResponse handleSinglePlanetRequest(const OnPlanetRequest&);
private:
    LoginResponse handle(const LoginRequest&);
    RegisterResponse handle(const RegisterRequest&);
    IStorageDb& storageDb;
    ITime& time;
};