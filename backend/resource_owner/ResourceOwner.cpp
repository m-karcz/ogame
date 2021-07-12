#include "ResourceOwner.hpp"
#include "LockResponse.hpp"

void ResourceOwner::checkPendingRequests()
{
    pendingRequests.remove_if([&, this](auto& request){
        if(canBeAcquired(request->players))
        {
            auto lockId = acquireLock(request->players);
            request.respond(LockResponse{
                .timestamp = time.getTimestamp(),
                .lockId = (int)lockId
            });
            return true;
        }
        return false;
    });
}

void ResourceOwner::consumeEvent(const Request<LockRequest>& request)
{
    pendingRequests.push_back(request);
    checkPendingRequests();
}

void ResourceOwner::consumeEvent(const Event<LockRelease>& release)
{
    for(auto& id : locked[release->lockId])
    {
        playersLocked.erase(id);
    }
    locked.erase(release->lockId);
    checkPendingRequests();
}

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

ResourceOwner::LockId ResourceOwner::acquireLock(const std::vector<PlayerId>& toBeAcquired)
{
    ++lastLockId;
    playersLocked.insert(toBeAcquired.begin(), toBeAcquired.end());
    locked[lastLockId] = toBeAcquired;
    return lastLockId;
}
