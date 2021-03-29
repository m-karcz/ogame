#pragma once

#include "IStorageDb.hpp"
#include <memory>

struct IStorageDbFactory
{
    virtual ~IStorageDbFactory() = default;
    virtual void cleanIfNeeded() = 0;
    virtual std::shared_ptr<IStorageDb> create() const = 0;
};
