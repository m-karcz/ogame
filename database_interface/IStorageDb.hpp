#pragma once
#include <memory>
#include "PlanetLocation.hpp"
#include "PlayerId.hpp"
#include "Storage.hpp"


struct ILazyBuildings;
struct IPlanetHandle;
struct ILazyResearchs;
struct ILazyBuildings;
struct ILazyStorage;
struct ILazyBuildingQueue;
struct UserCredentials;
struct IPlayerHandle;

struct IStorageDb
{
    virtual std::shared_ptr<IPlayerHandle> queryPlayer(const UserCredentials&) = 0;
    virtual std::shared_ptr<IPlayerHandle> queryPlayer(PlayerId) = 0;
    virtual bool registerPlayer(const UserCredentials&) = 0;
    virtual ~IStorageDb() = default;
};