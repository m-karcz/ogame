#pragma once
#include "IStorageDb.hpp"
#include <any>

namespace inMemory
{

struct StorageDb : ::IStorageDb
{
    std::shared_ptr<IPlayerHandle> queryPlayer(const UserCredentials&) override;
    std::shared_ptr<IPlayerHandle> queryPlayer(PlayerId) override;
    bool registerPlayer(const UserCredentials&) override;
    StorageDb();

    struct PlayerHandle;
    struct PlanetHandle;
private:
    auto& storage();
    std::any typeErasedStorage;
};

}