#include "IStorageDbFactory.hpp"
#include "StorageDb.hpp"

namespace inMemory
{

struct StorageDbFactory : ::IStorageDbFactory
{
    std::shared_ptr<IStorageDb> create() const override
    {
        return std::make_shared<inMemory::StorageDb>();
    }
};

}