#pragma once

#include "ISerializer.hpp"
#include "Configuration.hpp"
#include "Service.hpp"
#include "RnDService.hpp"
#include "IStorageDbFactory.hpp"
#include "IStorageDb.hpp"
#include <memory>

struct SingleInstance 
{
    SingleInstance(const ISerializer&, const Configuration&, ITime& time, IStorageDbFactory&);
    std::vector<uint8_t> process(const std::vector<uint8_t>&);
private:
    const Configuration& configuration;
    const ISerializer& serializer;
    std::shared_ptr<IStorageDb> storageDb;
    ITime& time;
    Service service;
    RnDService rndService;
};