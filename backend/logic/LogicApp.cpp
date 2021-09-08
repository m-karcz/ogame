#include "LogicApp.hpp"
#include <iostream>

LogicApp::LogicApp(IAsyncWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew>& workStealer,
                    IAsyncRequester<LockRequestNew,LockResponseNew>& resOwnConn,
                    IStorageDb& storageDb,
                    const Configuration& configuration)
    : fsm{storageDb, configuration, resOwnConn}
{
    resOwnConn.registerResponseHandler([this](auto event){
        fsm.process(std::move(event));});
    workStealer.registerWorkStealing([this](auto request){
        fsm.process(std::move(request));
        });
    workStealer.sendStartIndication(WorkStealerReady{});
}