#include "LogicApp.hpp"
#include <iostream>

LogicApp::LogicApp(IAsyncWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew>& workStealer,
                    IAsyncRequester<LockRequestNew,LockResponseNew>& resOwnConn,
                    IStorageDb& storageDb,
                    const Configuration& configuration)
    : fsm{storageDb, configuration, resOwnConn}
{
    resOwnConn.registerResponseHandler([this](auto event){
        std::cout << "received lock response" << std::endl;
        fsm.process(std::move(event));});
    workStealer.registerWorkStealing([this](auto request){
        std::cout << "stole request" << std::endl;
        fsm.process(std::move(request));
        });
    workStealer.sendStartIndication(WorkStealerReady{});
}