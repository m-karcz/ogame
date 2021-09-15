#include "ResourceOwner.hpp"
#include <iostream>

bool ResourceOwner::canBeAcquired(const std::vector<PlayerId>& toBeAcquired)
{
    for(auto& playerId : toBeAcquired)
    {
        if(playersLocked.count(playerId))
        {
            return false;
        }
    }
    return true;
}

void ResourceOwner::consume(Request<LockRequestNew, LockResponseNew> req)
{
    if(std::holds_alternative<LockReleaseNew>(req->data))
    {
        if(planetCreationLocked == req->instanceId)
        {
            planetCreationLocked = std::nullopt;
        }
        else
        {
            for(auto playerId : lockedMapping.extract(req->instanceId).mapped())
            {
                playersLocked.erase(playerId);
            }
        }
    }
    else
    {
        pendingRequests.push_back(std::move(req));
    }
    evaluatePendingRequests();
}
void ResourceOwner::respondSuccessfulLock(Request<LockRequestNew, LockResponseNew>& req)
{
    req.respond({
        .success = true,
        .timestamp = time.getTimestamp()
    });
}

void ResourceOwner::evaluatePendingRequests()
{
    pendingRequests.remove_if([&](auto& request){
        if(std::holds_alternative<LockPlanetCreation>(request->data))
        {
            if(not planetCreationLocked)
            {
                planetCreationLocked = request->instanceId;
                respondSuccessfulLock(request);
                return true;
            }
        }
        else if(auto* lockPlayersReq = std::get_if<LockPlayersNew>(&request->data))
        {
            if(canBeAcquired(lockPlayersReq->players))
            {
                lockedMapping[request->instanceId] = lockPlayersReq->players;
                respondSuccessfulLock(request);
                return true;
            }
        }
        return false;
    });
}

