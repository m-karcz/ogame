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

bool isWorkerReady(const WorkSharedPlanetRequest& req)
{
    return std::holds_alternative<WorkStealerReady>(req.data);
}

const OnPlanetResponseNew& getWorkResponse(const WorkSharedPlanetRequest& req)
{
    return std::get<OnPlanetResponseNew>(req.data);
}

template<typename...Types>
SerializationContainer<Types&...> containerize(Types&... toContainerize)
{
    return {toContainerize...};
}

int main()
{
    zmq::context_t context{1};

    ZmqPoller poller{};

    JsonTypedDeserializer<InternalGeneralRequest> generalRequestDeserializer;
    JsonTypedSerializer<InternalGeneralResponse> generalResponseSerializer;
    JsonTypedDeserializer<AuthenticatedOnPlanetRequest> authPlanetRequestDeserializer;
    JsonTypedSerializer<AuthenticatedOnPlanetRequest> authPlanetRequestSerializer;
    JsonTypedSerializer<WorkSharedPlanetRequest> workSharedPlanetRequestSerializer;
    TransformingSerializer<WorkStealerReady, WorkSharedPlanetRequest> workStealerSerializer{
        workSharedPlanetRequestSerializer,
        [](auto& ready)->WorkSharedPlanetRequest{return {. data = ready};}
    };
    TransformingSerializer<OnPlanetResponseNew, WorkSharedPlanetRequest> onPlanetResponseNewCombinedSerializer{
        workSharedPlanetRequestSerializer,
        [](auto& response)->WorkSharedPlanetRequest{return {. data = response};}
    };
    JsonTypedSerializer<OnPlanetResponseNew> onPlanetResponsePlainSerializer;

    JsonTypedDeserializer<WorkSharedPlanetRequest> workSharedPlanetRequestDeserializer;

    JsonTypedSerializer<LockRequestNew> lockRequestSerializer;
    JsonTypedSerializer<LockResponseNew> lockResponseSerializer;
    JsonTypedDeserializer<LockRequestNew> lockRequestDeserializer;
    JsonTypedDeserializer<LockResponseNew> lockResponseDeserializer;

    Configuration configuration = loadConfiguration(CONFIGURATION_FILE_DIR "Configuration.json");

    ZmqRouter<AuthenticatedOnPlanetRequest, OnPlanetResponseNew> workDealerForBalancer{context, configuration.addresses.workDealing, containerize(authPlanetRequestDeserializer, onPlanetResponsePlainSerializer)};
    ZmqRouter<WorkSharedPlanetRequest, AuthenticatedOnPlanetRequest> workStealerForBalancer{context, configuration.addresses.workStealing, containerize(authPlanetRequestSerializer, workSharedPlanetRequestDeserializer)};

    SimpleLoadBalancer<AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> loadBalancer{workDealerForBalancer, workStealerForBalancer};

    ZmqWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew> logicWorkStealer{context, configuration.addresses.workStealing, containerize(workStealerSerializer, onPlanetResponseNewCombinedSerializer, authPlanetRequestDeserializer)};

    sqlitedb::StorageDbFactory dbFactory{"testtest.db"};

    logger.debug("Loading configuration");

    dbFactory.cleanIfNeeded();

    auto db = dbFactory.create();



    ZmqDealer<LockRequestNew, LockResponseNew> lockRequestResOwnerDealer{context, configuration.addresses.resourceOwner, containerize(lockRequestSerializer, lockResponseDeserializer)};

    LogicApp logicApp{logicWorkStealer, lockRequestResOwnerDealer, *db, configuration};

    ZmqRouter<InternalGeneralRequest, InternalGeneralResponse> generalRequestRouter{context, configuration.addresses.generalRequest, containerize(generalRequestDeserializer, generalResponseSerializer)};

    AccountServiceApp accServiceApp{*db, generalRequestRouter};

    Time realTime{};

    ZmqRouter<LockRequestNew, LockResponseNew> lockRequestResOwnerRouter{context, configuration.addresses.resourceOwner, containerize(lockRequestDeserializer, lockResponseSerializer)};

    ResourceOwnerApp resourceOwnerApp{realTime, lockRequestResOwnerRouter};

    JsonTypedDeserializer<std::string> stringDeserializer;
    JsonTypedSerializer<std::string> stringSerializer;

    ZmqRouter<std::string, std::string> rndRouter{context, *configuration.addresses.rndMainAddress, containerize(stringSerializer, stringDeserializer)};

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