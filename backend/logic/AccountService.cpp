#include "AccountService.hpp"
#include "IPlayerHandle.hpp"

AccountService::AccountService(IStorageDb& storageDb) 
    : storageDb{storageDb}
{}

void AccountService::process(Request<LoginRequest, InternalLoginResponse> req)
{
    auto res = storageDb.queryPlayer(req->credentials);
    logger.debug("querying login for login: {}, password: {}", req->credentials.login, req->credentials.passcode);
    InternalLoginResponse resp{
        .playerId = std::nullopt
    };
    if(res)
    {
        logger.debug("setting id");
        resp.playerId = res->getId();
    }
    logger.debug("sending response");
    req.respond(resp);
}

void AccountService::process(Request<RegisterRequest, RegisterResponseNew> req)
{
    auto res = storageDb.registerPlayer(req->credentials);
    logger.debug("registering for for login: {}, password: {}", req->credentials.login, req->credentials.passcode);
    req.respond(RegisterResponseNew{.success = (bool)res});
}