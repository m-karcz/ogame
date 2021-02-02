#include "IStorageDbFactory.hpp"
#include <string_view>
#include "StorageDb.hpp"

namespace sqlite
{

struct StorageDbFactory : ::IStorageDbFactory
{
    StorageDbFactory(std::string_view path) : path{path}
    {}
    std::shared_ptr<IStorageDb> create() const override
    {
        return std::make_shared<sqlite::StorageDb>(path);
    }
    std::string_view path;
};

}