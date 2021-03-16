#pragma once

#include "IStorageDb.hpp"
#include "ITime.hpp"
#include "GeneralRequest.hpp"
#include "GeneralResponse.hpp"
#include "OnPlanetRequest.hpp"
#include "OnPlanetResponse.hpp"

struct Configuration;


struct Service
{
    Service(IStorageDb& storageDb, ITime& time, const Configuration& configuration) : storageDb{storageDb}, time{time}, configuration{configuration}
    {}
    GeneralResponse handleRequest(const GeneralRequest&);
    OnPlanetResponse handleSinglePlanetRequest(const OnPlanetRequest&);
private:
    LoginResponse handle(const LoginRequest&);
    RegisterResponse handle(const RegisterRequest&);
    IStorageDb& storageDb;
    ITime& time;
    const Configuration& configuration;
};