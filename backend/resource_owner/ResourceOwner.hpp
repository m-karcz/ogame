#pragma once

#include "LockRequest.hpp"
#include "LockRelease.hpp"
#include <map>
#include <set>
#include <list>
#include <vector>
#include "Event.hpp"
#include <cstdint>
#include "ITime.hpp"

struct ResourceOwner
{
    ResourceOwner(ITime& time) : time{time}
    {}
    void consumeEvent(const Request<LockRequest>&);
    void consumeEvent(const Event<LockRelease>&);
    void checkPendingRequests();
    bool canBeAcquired(const std::vector<PlayerId>&);
    using LockId = uint32_t;
    LockId acquireLock(const std::vector<PlayerId>&);

    struct PlayerIdComparator
    {
        bool operator()(const PlayerId& lhs, const PlayerId& rhs) const 
        {
            return lhs.id < rhs.id;
        }
    };
    
    ITime& time;
    std::map<LockId, std::vector<PlayerId>> locked;
    std::set<PlayerId, PlayerIdComparator> playersLocked;
    std::list<Request<LockRequest>> pendingRequests;
    LockId lastLockId = 0;
};
