#pragma once

#include "api/RnDRequestVariant.hpp"
#include "IStorageDb.hpp"
#include "ITime.hpp"
#include "GeneralRequest.hpp"
#include "GeneralResponse.hpp"
#include "OnPlanetRequest.hpp"
#include "OnPlanetResponse.hpp"



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