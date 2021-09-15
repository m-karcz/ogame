#pragma once

#include <map>
#include <set>
#include <list>
#include <vector>
#include "Event.hpp"
#include <cstdint>
#include "ITime.hpp"
#include "LockRequestNew.hpp"
#include <optional>
#include "LockResponseNew.hpp"

struct ResourceOwner
{
    ResourceOwner(ITime& time) : time{time}
    {}

    struct PlayerIdComparator
    {
        bool operator()(const PlayerId& lhs, const PlayerId& rhs) const 
        {
            return lhs.id < rhs.id;
        }
    };

    using InstanceId = int;

    std::list<Request<LockRequestNew, LockResponseNew>> pendingRequests;
    std::optional<InstanceId> planetCreationLocked;
    
    bool canBeAcquired(const std::vector<PlayerId>&);
    ITime& time;
    void consume(Request<LockRequestNew, LockResponseNew>);
    void evaluatePendingRequests();
    std::map<InstanceId, std::vector<PlayerId>> lockedMapping;
    std::set<PlayerId, PlayerIdComparator> playersLocked;
    void respondSuccessfulLock(Request<LockRequestNew, LockResponseNew>&);
};
