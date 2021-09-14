#include <iostream>

#include "ZmqHelper.hpp"
#include "SimpleLoadBalancer.hpp"
#include "JsonSerializer.hpp"

#include "AuthenticatedOnPlanetRequest.hpp"
#include "WorkSharedPlanetRequest.hpp"
#include "WorkStealerReady.hpp"
#include "StorageDbFactory.hpp"
#include "LoadConfiguration.hpp"
#include "Configuration.hpp"
#include "LogicApp.hpp"
#include "InternalGeneralRequest.hpp"
#include "InternalGeneralResponse.hpp"
#include "AccountServiceApp.hpp"
#include "ResourceOwnerApp.hpp"
#include "Time.hpp"
#include "RnDZmqCommandReceiver.hpp"
#include "Logger.hpp"
#include "JsonContainerMaker.hpp"
#include "WorkLoadHelperFunctions.hpp"

int main()
{
    zmq::context_t context{1};

    ZmqPoller poller{};

    JsonContainerMaker containerMaker;

    Configuration configuration = loadConfiguration(CONFIGURATION_FILE_DIR "Configuration.json");

    ZmqRouter<AuthenticatedOnPlanetRequest, OnPlanetResponseNew> workDealerForBalancer{context, configuration.addresses.workDealing, containerMaker};
    ZmqRouter<WorkSharedPlanetRequest, AuthenticatedOnPlanetRequest> workStealerForBalancer{context, configuration.addresses.workStealing, containerMaker};
    SimpleLoadBalancer<AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> loadBalancer{workDealerForBalancer, workStealerForBalancer};
    ZmqWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> logicWorkStealer{context, configuration.addresses.workStealing, containerMaker};

    sqlitedb::StorageDbFactory dbFactory{"testtest.db"};
    dbFactory.cleanIfNeeded();
    auto db = dbFactory.create();

    ZmqDealer<LockRequestNew, LockResponseNew> lockRequestResOwnerDealer{context, configuration.addresses.resourceOwner, containerMaker};
    LogicApp logicApp{logicWorkStealer, lockRequestResOwnerDealer, *db, configuration};

    ZmqRouter<InternalGeneralRequest, InternalGeneralResponse> generalRequestRouter{context, configuration.addresses.generalRequest, containerMaker};
    AccountServiceApp accServiceApp{*db, generalRequestRouter};

    Time realTime{};
    ZmqRouter<LockRequestNew, LockResponseNew> lockRequestResOwnerRouter{context, configuration.addresses.resourceOwner, containerMaker};
    ResourceOwnerApp resourceOwnerApp{realTime, lockRequestResOwnerRouter};

    ZmqRouter<std::string, std::string> rndRouter{context, *configuration.addresses.rndMainAddress, containerMaker};
    RnDZmqCommandReceiver rnd{configuration, rndRouter, realTime, *db};

    rndRouter.registerPoller(poller);
    logicWorkStealer.registerPoller(poller);
    generalRequestRouter.registerPoller(poller);
    lockRequestResOwnerRouter.registerPoller(poller);
    lockRequestResOwnerDealer.registerPoller(poller);
    workDealerForBalancer.registerPoller(poller);
    workStealerForBalancer.registerPoller(poller);
    try
    {
        poller.poll();
    }
    catch(...)
    {
        logger.error("died after exception");
    }
}