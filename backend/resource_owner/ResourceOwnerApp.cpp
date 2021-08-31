#include "ResourceOwnerApp.hpp"

ResourceOwnerApp::ResourceOwnerApp(ITime& time, IAsyncResponser<LockRequestNew, LockResponseNew>& lockReq)
    : owner{time}
{
    lockReq.registerRequestHandler([this](auto event){owner.consume(event);});
}