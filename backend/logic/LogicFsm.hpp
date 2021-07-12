#pragma once

#include "Event.hpp"
#include "OnPlanetRequestNew.hpp"
#include <memory>
#include "LockResponse.hpp"

struct IStorageDb;
struct Configuration;


struct LogicService
{
    struct Impl;
    LogicService(IStorageDb&, const Configuration&);
    ~LogicService();
    void process(Request<OnPlanetRequestNew>);
    void process(Request<LockResponse>);
    std::unique_ptr<Impl> impl;
};

