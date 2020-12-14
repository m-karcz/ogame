#pragma once

#include "IStorageDb.hpp"

#include <any>
#include <string_view>

namespace sqlite
{

    struct StorageDb : IStorageDb
    {
        struct PlayerHandle;
        struct PlanetHandle;

        StorageDb(std::string_view);

        std::shared_ptr<IPlayerHandle> queryPlayer(const UserCredentials&) override;
        std::shared_ptr<IPlayerHandle> queryPlayer(PlayerId) override;
        bool registerPlayer(const UserCredentials&) override;
    private:
/*        std::shared_ptr<ILazyStorage> getStorage(const PlanetHandle&);
        std::shared_ptr<ILazyResearchs> getResearchs(const PlayerHandle&);
        std::vector<std::shared_ptr<IPlanetHandle>> getPlanetList(const PlayerHandle&);*/
        auto& storage();
        std::any typeErasedStorage;
        void initDb(std::string_view);
    };
}