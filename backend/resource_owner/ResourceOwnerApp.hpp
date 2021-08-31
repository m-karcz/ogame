#pragma once

#include "ResourceOwner.hpp"
#include "Event.hpp"

struct ResourceOwnerApp
{
    ResourceOwnerApp(ITime& time, IAsyncResponser<LockRequestNew, LockResponseNew>& lockReq);

    ResourceOwner owner;
};