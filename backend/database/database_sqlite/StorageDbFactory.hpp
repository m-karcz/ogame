#include "StorageDb.hpp"
#include "IStorageDbFactory.hpp"

namespace sqlitedb
{

struct StorageDbFactory : ::IStorageDbFactory
{
    StorageDbFactory(std::string path) : path{path}
    {}
    void cleanIfNeeded() override
    {
        sqlitedb::StorageDb::cleanIfNeeded(path);
    }
    std::shared_ptr<IStorageDb> create() const override
    {
        return std::make_shared<sqlitedb::StorageDb>(std::move(path));
    }
private:
    std::string path;
};

}