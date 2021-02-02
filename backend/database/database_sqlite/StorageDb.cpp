#include "StorageDb.hpp"
#include <sqlite_orm.h>
#include <boost/hana/find_if.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/for_each.hpp>
#include <iostream>
#include "PlanetLocation.hpp"
#include "Buildings.hpp"
#include "NamedColumn.hpp"
#include "BuildingsWrapper.hpp"
#include "PlanetWrapper.hpp"
#include "BuildingQueueWrapper.hpp"
#include "UserCredentialsWrapper.hpp"
#include "PlayerHandle.hpp"
#include "TimestampSerializer.hpp"
#include "BuildingSerializer.hpp"
#include "BigNumSerializer.hpp"
#include "StorageWrapper.hpp"
#include "IPlanetHandle.hpp"
#include "CachedProductionWrapper.hpp"
#include <iostream>
#include <thread>
#include "Logger.hpp"

using namespace sqlite_orm;

namespace {
    template<typename T, typename Ret, typename U>
    Ret T::* upcast(Ret U::*ptr)
    {
        return static_cast<Ret T::*>(ptr);
    }
}

#define C(Type, Member) c(upcast<Type>(&Type::Member))



auto makeStorage(std::string_view path)
{
    const auto buildingsTable = makeTable<sqlite::BuildingQueueWrapper>("buildingQueue");
    const auto storageTable = makeTable<sqlite::StorageWrapper>("storage");
    const auto productionTable = makeTable<sqlite::CachedProductionWrapper>("cachedProduction");
    return make_storage(std::string{path},
                                   makeTable<sqlite::UserCredentialsWrapper>("users"),
                                   makeTable<sqlite::PlanetWrapper>("planets"),
                                   makeTable<sqlite::BuildingsWrapper>("buildings"),
                                   buildingsTable,
                                   storageTable,
                                   productionTable
                               );
}

using StorageT = decltype(makeStorage(""));

namespace sqlite
{
    auto& StorageDb::storage()
    {
        return std::any_cast<StorageT&>(typeErasedStorage);
    }
    struct StorageDb::PlanetHandle : IPlanetHandle
    {
        PlanetHandle(StorageT& storageDb, int id) : db{storageDb}, planetId{id}
        {}
        Buildings getBuildings() override
        {
            try
            {
                logger.debug("Querying buildings");
                return db.get<BuildingsWrapper>(planetId);
            }
            catch(std::exception& ex)
            {
                logger.error("Error in querying buildings: {}", ex.what());
                throw;
            }
        }
        CachedProduction getCachedProduction() override
        {
            try
            {
                logger.debug("Querying production");
                return db.get<CachedProductionWrapper>(planetId);
            }
            catch(std::exception& ex)
            {
                logger.error("Error in querying production : {}", ex.what());
                throw;
            }

        }
        void setNewCachedProduction(const CachedProduction& production) override
        {
            logger.debug("Setting new cachedProduction");
            CachedProductionWrapper wrapper{production};
            wrapper.planetId = planetId;
            db.replace(wrapper);
        }
        void setNewStorage(const Storage& storage) override
        {
            logger.debug("Setting new storage");
            StorageWrapper wrapper{storage};
            wrapper.planetId = planetId;
            db.replace(wrapper);
        }
        std::optional<BuildingQueue> getBuildingQueue() override
        {
            logger.debug("Querying building queue");
            return db.get_optional<BuildingQueueWrapper>(planetId);
        }

        void incrementBuildingLevel(const Building& building) override
        {
            logger.debug("Incrementing building level for {}", building.fieldName);
            auto wrapper = db.get<BuildingsWrapper>(planetId);
            building.increment(wrapper);
            db.replace(wrapper);
        }
        Storage getStorage() override
        {
            logger.debug("Getting storage");
            return db.get<StorageWrapper>(planetId);
        }
        int getBuildingLevel(const Building& building) override
        {
            logger.debug("Getting building level for {}", building.fieldName);
            auto buildings = db.get<BuildingsWrapper>(planetId);
            return buildings.*(building.field);
        }
        void queueBuilding(const BuildingQueue& building) override
        {
            logger.debug("Queueing {} to build", building.building.fieldName);
            BuildingQueueWrapper wrapper{building};
            wrapper.planetId = planetId;
            db.insert(wrapper);
        }
        void dequeueBuilding(const BuildingQueue& building) override
        {
            (void)building;
            db.remove<BuildingQueueWrapper>(planetId);
        }
        int planetId;
        StorageT& db;
    };


    struct StorageDb::PlayerHandle : IPlayerHandle
    {
        PlayerHandle(StorageT& db, PlayerId playerId) : db{db}, playerId{playerId}
        {}
        PlayerId getId() const override
        {
            return playerId;
        }
        std::vector<PlanetLocation> getPlanetList() override
        {
            auto planets = db.get_all<PlanetWrapper>(where(c(&PlanetWrapper::playerId) == playerId.id));
            return {planets.begin(), planets.end()};
        }
        std::shared_ptr<IPlanetHandle> getPlanet(const PlanetLocation& loc) override
        {
            auto id = db.select(&PlanetWrapper::planetId, where(C(PlanetWrapper,galaxy)   == loc.galaxy
                                                               and  C(PlanetWrapper,solar)    == loc.solar
                                                               and  C(PlanetWrapper,position) == loc.position
                                                               and  C(PlanetWrapper,playerId) == playerId.id));

            if(id.size())
            {
                return std::make_shared<PlanetHandle>(db, id.front());
            }
            return nullptr;
        }
        bool createPlanet(const PlanetLocation& planet, const Timestamp& createdAt) override
        {
            PlanetWrapper wrapper{planet};
            wrapper.playerId = playerId.id;
            wrapper.planetId = -1;

            try
            {
                auto planetId = db.insert(wrapper);
                createStorage(planetId, createdAt);
                createBuildings(planetId);
                createCachedProduction(planetId);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }
        std::shared_ptr<ILazyResearchs> getResearchs() override;
    private:
        void createStorage(int planetId, const Timestamp& createdAt)
        {
            StorageWrapper storageWrapper{Storage{.lastUpdatedAt = createdAt, .metal = 500, .crystal = 500, .deuter = 0}};
            storageWrapper.planetId = planetId;
            db.insert(storageWrapper);
        }
        void createBuildings(int planetId)
        {
            BuildingsWrapper wrapper{};
            wrapper.planetId = planetId;
            auto givenId = db.insert(wrapper);
            logger.debug("Added buildings with id {} and planetId = {}", givenId, planetId);
            db.update_all(set(c(&BuildingsWrapper::planetId) = planetId), where(c(&BuildingsWrapper::planetId) == givenId));
        }
        void createCachedProduction(int planetId)
        {
            CachedProductionWrapper wrapper{};
            wrapper.planetId = planetId;
            auto givenId = db.insert(wrapper);
            logger.debug("Added empty cachedProduction with id {} and planetId = {}", givenId, planetId);
            db.update_all(set(c(&CachedProductionWrapper::planetId) = planetId), where(c(&CachedProductionWrapper::planetId) == givenId));
        }
        StorageT& db;
        PlayerId playerId;
    };

    std::shared_ptr<ILazyResearchs> StorageDb::PlayerHandle::getResearchs()
    {
        return nullptr;
    }
    StorageDb::StorageDb(std::string_view path)
    {
        initDb(path);
    }
    void StorageDb::initDb(std::string_view path)
    {
        typeErasedStorage = makeStorage(path);
        storage().sync_schema();
        storage().open_forever();
    }

    std::shared_ptr<IPlayerHandle> StorageDb::queryPlayer(const UserCredentials& credentials)
    {
        auto user = storage().select(&UserCredentialsWrapper::id, where(C(UserCredentialsWrapper,login)    == credentials.login
                                                                    and C(UserCredentialsWrapper,passcode) == credentials.passcode));
        if(user.size())
        {
            return queryPlayer({user.front()});
        }
        return nullptr;
    }

    std::shared_ptr<IPlayerHandle> StorageDb::queryPlayer(PlayerId playerId)
    {
        return std::make_shared<PlayerHandle>(storage(), playerId);
    }

    bool StorageDb::registerPlayer(const UserCredentials& credentials)
    {
        try
        {
            UserCredentialsWrapper wrapper{credentials, -1};
            auto id = storage().insert(wrapper);
            std::cerr << id << std::endl;
            return true;
        }
        catch(...)
        {
            return false;
        }
    }
}