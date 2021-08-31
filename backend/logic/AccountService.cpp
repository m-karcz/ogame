#include "AccountService.hpp"
#include "IPlayerHandle.hpp"

AccountService::AccountService(IStorageDb& storageDb) 
    : storageDb{storageDb}
{}

void AccountService::process(Request<LoginRequest, InternalLoginResponse> req)
{
    auto res = storageDb.queryPlayer(req->credentials);
    InternalLoginResponse resp{
        .playerId = std::nullopt
    };
    if(res)
    {
        resp.playerId = res->getId();
    }
    req.respond(resp);
}

void AccountService::process(Request<RegisterRequest, RegisterResponseNew> req)
{
    auto res = storageDb.registerPlayer(req->credentials);
    req.respond(RegisterResponseNew{.success = (bool)res});
}