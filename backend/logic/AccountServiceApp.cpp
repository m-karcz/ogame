#include "AccountServiceApp.hpp"
#include "InternalGeneralRequest.hpp"
#include "InternalGeneralResponse.hpp"
#include <iostream>

InternalLoginResponse getRespT(const LoginRequest&);
RegisterResponseNew getRespT(const RegisterRequest&);

AccountServiceApp::AccountServiceApp(IStorageDb& storageDb, IAsyncResponser<InternalGeneralRequest, InternalGeneralResponse>& responser)
    : service{storageDb}
{
    responser.registerRequestHandler([&](Request<InternalGeneralRequest, InternalGeneralResponse> request){
        std::cout << "Received general request" << std::endl;
        std::visit([&](auto req){
            using ReqT = decltype(req);
            using RespT = decltype(getRespT(req));
            Event<ReqT> ev{std::make_shared<PlainEventHolder<ReqT>>(req)};
            Request<ReqT, RespT> typedReq{std::move(ev), std::make_shared<StdFunctionResponseHandler<RespT>>([requestCopy=request](auto& resp) mutable {
                requestCopy.respond(InternalGeneralResponse{.data = resp});
                std::cout << "Responded general request" << std::endl;
            })};
            service.process(typedReq);
        }, request->data);
    });
}