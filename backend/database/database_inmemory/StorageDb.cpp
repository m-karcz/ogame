#include "PlanetLocation.hpp"
#include "UserCredentials.hpp"
#include <functional>
#include <map>
#include "StorageDb.hpp"
#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "Buildings.hpp"
#include "BuildingQueue.hpp"
#include "Researchs.hpp"
#include "CachedProduction.hpp"
#include "Building.hpp"
#include "BuildingQueue.hpp"


namespace inMemory
{
using IndexType = int;

struct PlanetLocationWrapper : PlanetLocation
{
    using Base = PlanetLocation;
    //BOOST_HANA_DEFINE_STRUCT()
    IndexType playerId;
    IndexType planetId;
    
    constexpr static auto key = &PlanetLocationWrapper::planetId;
};

struct UserCredentialsWrapper : UserCredentials
{
    using Base = UserCredentials;
    IndexType playerId;

    constexpr static auto key = &UserCredentialsWrapper::playerId;
};

struct StorageWrapper : Storage
{
    using Base = Storage;
    IndexType planetId;

    constexpr static auto key = &StorageWrapper::planetId;
};

struct BuildingsWrapper : Buildings
{
    using Base = Buildings;
    IndexType planetId;

    constexpr static auto key = &BuildingsWrapper::planetId;
};

struct CachedProductionWrapper : CachedProduction
{
    using Base = CachedProduction;
    IndexType planetId;

    constexpr static auto key = &CachedProductionWrapper::planetId;
};

struct BuildingQueueWrapper : BuildingQueue
{
    using Base = BuildingQueue;
    IndexType planetId;

    constexpr static auto key = &BuildingQueueWrapper::planetId;
};

template<typename RowType>
struct IndexedTable
{
    IndexType lastIndex = 0;
    std::map<IndexType, RowType> map{};
    IndexType insert(const RowType& elem)
    {
        auto index = lastIndex++;
        map[index] = elem;
        (map[index].*(RowType::key)) = index;
        return index;
    }
    IndexType insertAlreadyIndiced(const RowType& elem)
    {
        auto index = elem.*RowType::key;
        map[index] = elem;
        return index;
    }
    void replace(const RowType& elem)
    {
        auto index = elem.*RowType::key;
        auto it = map.find(index);
        if(it != map.end())
        {
            it->second = elem;
            return;
        }
        throw std::runtime_error("could not find item for replace");
    }
    void remove(IndexType index)
    {
        auto it = map.find(index);
        if(it != map.end())
        {
            map.erase(it);
            return;
        }
        throw std::runtime_error("cannot find item to remove");
    }
    const RowType* query(IndexType index)
    {
        auto it = map.find(index);
        if(it != map.end())
        {
            return &it->second;
        }
        return nullptr;
        //throw std::runtime_error("couldn't find item to query");
    }
    std::vector<RowType> queryAll(std::function<bool(const RowType&)> pred)
    {
        std::vector<RowType> ret;
        for(auto& [index, elem] : map)
        {
            if(pred(elem))
            {
                ret.push_back(elem);
            }
        }
        return ret;
    }
    const RowType* queryIf(std::function<bool(const RowType&)> pred) //to be replaced with function ref
    {
        auto it = std::find_if(map.begin(), map.end(), [&](auto& pair){return pred(pair.second);});
        if(it != map.end())
        {
            return &it->second;
        }
        return nullptr;//throw std::runtime_error("couldn't find item to query");
    }
};

struct Database
{
    IndexedTable<UserCredentialsWrapper> users;
    IndexedTable<PlanetLocationWrapper> planets;
    IndexedTable<StorageWrapper> storages;
    IndexedTable<BuildingsWrapper> buildings;
    IndexedTable<BuildingQueueWrapper> buildingQueue;
    IndexedTable<CachedProductionWrapper> cachedProduction;
};

static auto makeDb()
{
    return Database{};
}

auto& StorageDb::storage()
{
    return std::any_cast<Database&>(typeErasedStorage);
}


StorageDb::StorageDb()
{
    typeErasedStorage = makeDb();
}

struct StorageDb::PlanetHandle : ::IPlanetHandle
{
    PlanetHandle(int planetId, Database& db) :
        planetId{planetId}, db{db}
    {}

    Buildings getBuildings() override
    {
        return *db.buildings.query(planetId);
    }
    std::optional<BuildingQueue> getBuildingQueue() override
    {
        auto* queue = db.buildingQueue.query(planetId);

        if(queue)
        {
            return *queue;
        }
        return std::nullopt;
    }
    void incrementBuildingLevel(const Building& building) override
    {
        auto wrapper = *db.buildings.query(planetId);
        building.increment(wrapper);
        db.buildings.replace(wrapper);
    }
    Storage getStorage() override
    {
        return *db.storages.query(planetId);
    }
    void setNewStorage(const Storage& newStorage) override
    {
        StorageWrapper wrapper{newStorage};
        wrapper.planetId = planetId;
        db.storages.replace(wrapper);
    }
    int getBuildingLevel(const Building& building) override
    {
        auto wrapper = *db.buildings.query(planetId);
        return wrapper.*building.field;
    }
    void queueBuilding(const BuildingQueue& queueToPut) override
    {
        BuildingQueueWrapper wrapper{queueToPut};
        wrapper.planetId = planetId;
        db.buildingQueue.insertAlreadyIndiced(wrapper);
    }
    void dequeueBuilding(const BuildingQueue& toDequeue) override
    {
        db.buildingQueue.remove(planetId);
    }
    CachedProduction getCachedProduction() override
    {
        return *db.cachedProduction.query(planetId);
    }
    void setNewCachedProduction(const CachedProduction& newCachedProduction) override
    {
        CachedProductionWrapper wrapper{newCachedProduction};
        wrapper.planetId = planetId;
        db.cachedProduction.insertAlreadyIndiced(wrapper);
    }
private:
    int planetId;
    Database& db;
};

struct StorageDb::PlayerHandle : ::IPlayerHandle
{
    PlayerHandle(PlayerId id, Database& db) :
        playerId{id}, db{db}
        {}
    PlayerId getId() const
    {
        return playerId;
    }
    std::vector<PlanetLocation> getPlanetList() override
    {
        auto planets = db.planets.queryAll([=](auto& elem){return elem.playerId == playerId.id;});
        return {planets.begin(), planets.end()};
    }
    std::shared_ptr<IPlanetHandle> getPlanet(const PlanetLocation& planet) override
    {
        auto* planetEntry = db.planets.queryIf([=](auto& elem){return elem.position == planet.position and elem.solar == planet.solar and elem.galaxy == planet.galaxy;});
        return std::make_shared<PlanetHandle>(planetEntry->planetId, db);
    }
    bool createPlanet(const PlanetLocation& location, const Timestamp& createdAt) override
    {
        PlanetLocationWrapper wrapper{location};
        wrapper.playerId = playerId.id;
        try
        {
            auto planetId = db.planets.insert(wrapper);
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
    std::shared_ptr<ILazyResearchs> getResearchs()
    {
        throw "no elo";
    }
private:
    void createStorage(int planetId, const Timestamp& createdAt)
    {
        StorageWrapper wrapper{
            Storage{
                .lastUpdatedAt = createdAt,
                .metal = 500,
                .crystal = 500,
                .deuter = 0
            },
            planetId = planetId
        };
        db.storages.insertAlreadyIndiced(wrapper);
    }
    void createBuildings(int planetId)
    {
        BuildingsWrapper wrapper = {};
        wrapper.planetId = planetId;
        db.buildings.insertAlreadyIndiced(wrapper);
    }
    void createCachedProduction(int planetId)
    {
        CachedProductionWrapper wrapper = {};
        wrapper.planetId = planetId;
        db.cachedProduction.insertAlreadyIndiced(wrapper);
    }
    PlayerId playerId;
    Database& db;
};


std::shared_ptr<IPlayerHandle> StorageDb::queryPlayer(const UserCredentials& credentials)
{
    auto& db = storage();
    auto* wrapper = db.users.queryIf([&](auto& entry){return entry.login == credentials.login
                                                         and entry.passcode == credentials.passcode;});

    if(wrapper)
    {
        return std::make_shared<PlayerHandle>(PlayerId{wrapper->playerId}, db);
    }
    return nullptr;
}

std::shared_ptr<IPlayerHandle> StorageDb::queryPlayer(PlayerId playerId)
{
    return std::make_shared<PlayerHandle>(playerId, storage());
}

bool StorageDb::registerPlayer(const UserCredentials& credentials)
{
    auto& db = storage();
    if(not db.users.queryIf([&](auto& entry){return entry.login == credentials.login;}))
    {
        db.users.insert({credentials});
        return true;
    }
    return false;
}


}