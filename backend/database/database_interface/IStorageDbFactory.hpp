#pragma once

#include "IStorageDb.hpp"
#include <memory>

struct IStorageDbFactory
{
    virtual ~IStorageDbFactory() = default;
    virtual std::shared_ptr<IStorageDb> create() const = 0;
};
