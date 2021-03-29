#include "StorageDb.hpp"
#include "Buildings.hpp"
#include "PlanetLocation.hpp"
#include "Researchs.hpp"
#include "CachedProduction.hpp"
#include "ProductionPercentages.hpp"
#include "Storage.hpp"
#include "IPlanetHandle.hpp"
#include "IPlayerHandle.hpp"
#include "MakeTable.hpp"
#include "BuildingQueue.hpp"
#include <optional>
#include "UserCredentials.hpp"
#include "Logger.hpp"
#include "Reflection.hpp"
#include <filesystem>
#include <cstdlib>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

namespace sqlitedb
{
namespace
{
void fireAndForget(sqlite3* handle, std::string query)
{
    logger.debug(query);
    if(query.back() != ';')
    query += ";";
    sqlite3_exec(handle,
                query.c_str(),
                +[](void*, int, char**, char**)->int{
                    return 0;
                },
                nullptr,
                nullptr);
}

template<typename T>
std::string serialize(const T& value)
{
    return Serialization<std::decay_t<T>>::serialize(value);
}

template<typename T>
void deserializeAllFields(T& obj, char** values)
{
    size_t i = 0;
    forEachField(obj, [&](auto& field){
        field = Serialization<std::decay_t<decltype(field)>>::deserialize(values[i++]);
    });
}

template<typename T>
T deserializeAllFields(char** values)
{
    T obj;
    deserializeAllFields(obj, values);
    return obj;
}

template<typename T>
std::string commaSeparatedFieldNames()
{
    std::string query;
    forEachFieldName<T>([&](auto name){
        query += name;
        query += ',';
    });
    query.pop_back();
    return query;
}

template<typename T>
std::string commaSeparatedFieldValues(const T& obj)
{
    std::string query;
    forEachField(obj, [&](auto& field){
        query += serialize(field);
        query += ",";
    });
    query.pop_back();
    return query;
}

template<typename T>
std::optional<T> queryPrimaryKeyed(sqlite3* handle, int key, std::string tableName = std::string{getTypeName<T>()})
{
    using namespace boost;
    std::string query = "SELECT ";
    query += commaSeparatedFieldNames<T>();
    query += " FROM ";
    query += tableName;
    query += " WHERE id = ";
    query += std::to_string(key);

    std::optional<T> ret;

    logger.debug(query);
    sqlite3_exec(handle,
                query.c_str(),
                +[](void* data, int size, char** values, char** column)->int{
                    *static_cast<std::optional<T>*>(data) = deserializeAllFields<T>(values);
                    return 0;
                },
                &ret,
                nullptr);
    return ret;
}
template<typename T>
void initializePrimaryKeyed(sqlite3* handle, int key, const T& value, std::string tableName = std::string{getTypeName<T>()})
{
    std::string query = "INSERT INTO " + tableName + " (id";
    query += ",";
    query += commaSeparatedFieldNames<T>();
    query += (") VALUES (" + std::to_string(key));
    query += ",";
    query += commaSeparatedFieldValues(value);
    query += ")";
    fireAndForget(handle, query);
}
template<typename T>
void updatePrimaryKeyed(sqlite3* handle, int key, const T& newValue, std::string tableName = std::string{getTypeName<T>()})
{
    std::string query = "UPDATE " + tableName + " SET ";

    forEachNamedField(newValue, [&](std::string name, auto& field){
        query += (name + " = " + serialize(field) + ",");
    });
    query.pop_back();

    query += (" WHERE id = " + std::to_string(key));

    fireAndForget(handle, query);
}
template<typename T>
void deletePrimaryKeyed(sqlite3* handle, int key, std::string tableName = std::string{getTypeName<T>()})
{
    std::string query = "DELETE FROM " + tableName + " WHERE id = " + std::to_string(key);

    fireAndForget(handle, query);
}

std::optional<PlayerId> queryPlayerBy(sqlite3* handle, PlayerId playerId)
{
    std::string query = "SELECT id FROM " + std::string{getTypeName<UserCredentials>()} + " WHERE id = " + Serialization<int>::serialize(playerId.id);
    query += ";";
    std::optional<PlayerId> ret;
    logger.debug(query);
    sqlite3_exec(handle,
                query.c_str(),
                +[](void* data, int size, char** values, char** column)->int{
                    if(size < 1)
                    {
                        return 0;
                    }
                    *static_cast<std::optional<PlayerId>*>(data) = PlayerId{Serialization<int>::deserialize(values[0])};
                    return 0;
                },
                &ret,
                nullptr);
    return ret;
}

std::optional<PlayerId> queryPlayerBy(sqlite3* handle, const UserCredentials& cred)
{
    std::string query = "SELECT id FROM " + std::string{getTypeName<UserCredentials>()} + " WHERE login = ";
    query += Serialization<std::string>::serialize(cred.login);
    query += " AND passcode = ";
    query += Serialization<std::string>::serialize(cred.passcode);
    query += ";";
    logger.debug(query);
    std::optional<PlayerId> ret;
    sqlite3_exec(handle,
                query.c_str(),
                +[](void* data, int size, char** values, char** column)->int{
                    *static_cast<std::optional<PlayerId>*>(data) = PlayerId{Serialization<int>::deserialize(values[0])};
                    return 0;
                },
                &ret,
                nullptr);
    logger.debug("tried to query by user credentials: {}", (bool)ret);
    return ret;
}

bool registerPlayer(sqlite3* handle, const UserCredentials& cred)
{
    std::string query = "SELECT id FROM " + std::string{getTypeName<UserCredentials>()} + " WHERE login = ";
    query += Serialization<std::string>::serialize(cred.login);
    std::optional<PlayerId> id;
    logger.debug(query);
    sqlite3_exec(handle,
                query.c_str(),
                +[](void* data, int size, char** values, char** column)->int{
                    *static_cast<std::optional<PlayerId>*>(data) = PlayerId{Serialization<int>::deserialize(values[0])};
                    return 0;
                },
                &id,
                nullptr);
    if(id)
    {
        return false;
    }

    std::string query2 = "INSERT INTO " + std::string{getTypeName<UserCredentials>()} + " (login, passcode) ";

    query2 += "VALUES (";

    query2 += Serialization<std::string>::serialize(cred.login);
    query2 += ",";
    query2 += Serialization<std::string>::serialize(cred.passcode);
    
    query2 += ")";

    fireAndForget(handle, query2);

    return true;
}
void makePlanetTable(sqlite3* handle)
{
    using namespace boost;
    std::string query = "CREATE TABLE IF NOT EXISTS " + std::string{getTypeName<PlanetLocation>()} + "(";
    query += "planetId INTEGER PRIMARY KEY AUTOINCREMENT";
    query += ",";
    query += "playerId INTEGER";
    hana::for_each(hana::accessors<PlanetLocation>(), [&](auto accessor){
        query += ", ";
        query += hana::first(accessor).c_str();
        query += " ";
        query += serializationRepresentation<decltype(hana::second(accessor)(std::declval<PlanetLocation>()))>();
    });
    query += ");";

    fireAndForget(handle, query);
}
void createPlanet(sqlite3* handle, PlayerId playerId, const PlanetLocation& location)
{
    std::string query = "INSERT INTO " + std::string{getTypeName<PlanetLocation>()} + " (playerId,";

    query += commaSeparatedFieldNames<PlanetLocation>();
    query += ") VALUES (";
    query += Serialization<int>::serialize(playerId.id);
    query += ",";
    query += commaSeparatedFieldValues(location);
    query += ")";

    fireAndForget(handle, query);
}
std::optional<int> getPlanetId(sqlite3* handle, const PlanetLocation& location)
{
    using namespace boost;
    std::string query = "SELECT planetId FROM " + std::string{getTypeName<PlanetLocation>()} + " WHERE ";

    forEachNamedField(location, [&](std::string name, auto& field){
        query += (name + " = " + serialize(field) + " AND ");
    });

    query.pop_back();
    query.pop_back();
    query.pop_back();
    query.pop_back();
    query.pop_back();

    std::optional<int> ret;
    logger.debug(query);
    sqlite3_exec(handle,
                query.c_str(),
                +[](void* data, int size, char** values, char** column)->int{
                    *static_cast<std::optional<int>*>(data) = Serialization<int>::deserialize(values[0]);
                    return 0;
                },
                &ret,
                nullptr);
    return ret;
}
std::vector<PlanetLocation> queryPlanets(sqlite3* handle, PlayerId playerId)
{
    std::vector<PlanetLocation> ret;
    std::string query = "SELECT ";
    query += commaSeparatedFieldNames<PlanetLocation>();
    query += " FROM " + std::string{getTypeName<PlanetLocation>()} + " WHERE playerId = " + Serialization<int>::serialize(playerId.id) + ";";

    logger.debug(query);

    sqlite3_exec(handle,
                 query.c_str(),
                +[](void* data, int size, char** values, char** column)->int{
                    static_cast<decltype(ret)*>(data)->push_back(deserializeAllFields<PlanetLocation>(values));
                    return 0;
                },
                &ret,
                nullptr
                );
    return ret;
}
constexpr int DB_VERSION = 2;
void makeDbVersionTable(sqlite3* handle)
{
    fireAndForget(handle, "CREATE TABLE IF NOT EXISTS DbVersion (value INTEGER);");
}
int queryDbVersion(sqlite3* handle)
{
    int version = DB_VERSION;
    sqlite3_exec(handle,
                 "SELECT value FROM DbVersion;",
                 +[](void* data, int size, char** values, char** column)->int{
                    *static_cast<int*>(data) = Serialization<int>::deserialize(values[0]);
                    return 0;
                 },
                 &version,
                 nullptr
                 );
    return version;
}
void setDbVersion(sqlite3* handle, int version = DB_VERSION)
{
    fireAndForget(handle, "DELETE FROM DbVersion");
    fireAndForget(handle, std::string{""} + "INSERT INTO DbVersion (value) VALUES (" + Serialization<int>::serialize(version) + ");");
}
}

StorageDb::StorageDb(std::string path) : path{path}
{
    initDb();
}

void StorageDb::initDb()
{
    constexpr bool autoincrement = true;
    sqlite3_open(path.c_str(), &handle);
    makePrimaryKeyedTable<UserCredentials>(handle, std::string{getTypeName<UserCredentials>()}, autoincrement);
    makePlanetTable(handle);
    makePrimaryKeyedTable<Buildings>(handle);
    makePrimaryKeyedTable<Researchs>(handle);
    makePrimaryKeyedTable<Storage>(handle);
    makePrimaryKeyedTable<BuildingQueue>(handle);
    makePrimaryKeyedTable<CachedProduction>(handle);
    makePrimaryKeyedTable<ProductionPercentages>(handle);
}

void StorageDb::cleanIfNeeded(const std::string& path)
{
    logger.debug("Clearing database");

    sqlite3* handle;

    sqlite3_open(path.c_str(), &handle);
    makeDbVersionTable(handle);
    int actual = queryDbVersion(handle);
    sqlite3_close(handle);

    bool removalNeed = actual < DB_VERSION;

    if(removalNeed)
    {
        #ifdef __EMSCRIPTEN__
        EM_ASM({
            window.indexedDB.deleteDatabase("/sqlite");
            window.location.reload(true);
        });
        #else
        std::filesystem::rename(path, path + ".removed");
        std::filesystem::remove(path + ".removed");
        #endif
        //initDb();
    }
    sqlite3_open(path.c_str(), &handle);
    makeDbVersionTable(handle);
    setDbVersion(handle, DB_VERSION);
    sqlite3_close(handle);
}
void StorageDb::simulateVersionBreak()
{
    setDbVersion(handle, 0);
}

struct StorageDb::PlanetHandle : ::IPlanetHandle
{
    PlanetHandle(sqlite3* handle, int id) : handle{handle}, id{id}
    {}

    Buildings getBuildings() override
    {
        return queryPrimaryKeyed<Buildings>(handle, id).value();
    }
    std::optional<BuildingQueue> getBuildingQueue() override
    {
        return queryPrimaryKeyed<BuildingQueue>(handle, id);
    }
    void incrementBuildingLevel(const Building& building) override
    {
        auto buildings = getBuildings();
        building.increment(buildings);
        updatePrimaryKeyed(handle, id, buildings);
    }
    Storage getStorage() override
    {
        return queryPrimaryKeyed<Storage>(handle, id).value();
    }
    void setNewStorage(const Storage& newValue) override
    {
        updatePrimaryKeyed(handle, id, newValue);
    }
    int getBuildingLevel(const Building& building) override
    {
        return building.value(getBuildings());
    }
    void queueBuilding(const BuildingQueue& queue) override
    {
        initializePrimaryKeyed(handle, id, queue);
    }
    void dequeueBuilding(const BuildingQueue&) override
    {
        deletePrimaryKeyed<BuildingQueue>(handle, id);
    }
    CachedProduction getCachedProduction() override
    {
        return queryPrimaryKeyed<CachedProduction>(handle, id).value();
    }
    void setNewCachedProduction(const CachedProduction& newValue) override
    {
        updatePrimaryKeyed(handle, id, newValue);
    }
    ProductionPercentages getProductionPercentages() override
    {
        return queryPrimaryKeyed<ProductionPercentages>(handle, id).value();
    }
    void setNewProductionPercentages(const ProductionPercentages& newValue) override
    {
        updatePrimaryKeyed(handle, id, newValue);
    }
private:
    int id;
    sqlite3* handle;
};

struct StorageDb::PlayerHandle : ::IPlayerHandle
{
    PlayerHandle(sqlite3* handle, PlayerId id) : handle{handle}, id{id}
    {}
    PlayerId getId() const
    {
        return id;
    }
    std::vector<PlanetLocation> getPlanetList()
    {
        return queryPlanets(handle, id);
    }
    std::shared_ptr<IPlanetHandle> getPlanet(const PlanetLocation& location)
    {
        if(auto planetId = getPlanetId(handle, location))
        {
            return std::make_shared<PlanetHandle>(handle, *planetId);
        }
        return nullptr;
    }
    bool createPlanet(const PlanetLocation& location, const Timestamp& timestamp)
    {
        if(auto planetId = getPlanetId(handle, location))
        {
            return false;
        }
        ::sqlitedb::createPlanet(handle, id, location);
        auto planetId = getPlanetId(handle, location);
        initializePrimaryKeyed(handle, *planetId, Storage{.lastUpdatedAt = timestamp, .metal = 500, .crystal = 500, .deuter = 0});
        initializePrimaryKeyed(handle, *planetId, Buildings{});
        initializePrimaryKeyed(handle, *planetId, CachedProduction{});
        initializePrimaryKeyed(handle, *planetId, ProductionPercentages{100, 100, 100, 100, 100, 100});
        return true;
    }
    std::shared_ptr<ILazyResearchs> getResearchs()
    {
        return nullptr;
    }
private:
    sqlite3* handle;
    PlayerId id;
};


std::shared_ptr<IPlayerHandle> StorageDb::queryPlayer(const UserCredentials& cred)
{
    if(auto id = queryPlayerBy(handle, cred))
    {
        return std::make_shared<StorageDb::PlayerHandle>(handle, *id);
    }
    return nullptr;
}
std::shared_ptr<IPlayerHandle> StorageDb::queryPlayer(PlayerId playerId)
{
    if(auto id = queryPlayerBy(handle, playerId))
    {
        return std::make_shared<StorageDb::PlayerHandle>(handle, *id);
    }
    return nullptr;
}
bool StorageDb::registerPlayer(const UserCredentials& cred)
{
    return ::sqlitedb::registerPlayer(handle, cred);
}
}