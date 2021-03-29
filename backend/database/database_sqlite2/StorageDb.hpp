#pragma once

#include "IStorageDb.hpp"

struct sqlite3;

namespace sqlitedb
{
struct StorageDb : ::IStorageDb
{
    std::shared_ptr<IPlayerHandle> queryPlayer(const UserCredentials&) override;
    std::shared_ptr<IPlayerHandle> queryPlayer(PlayerId) override;
    bool registerPlayer(const UserCredentials&) override;
    void simulateVersionBreak() override;
    static void cleanIfNeeded(const std::string&);
    StorageDb(std::string path);

    struct PlayerHandle;
    struct PlanetHandle;
private:
    void initDb();
    std::string path;
    sqlite3* handle;
};
}