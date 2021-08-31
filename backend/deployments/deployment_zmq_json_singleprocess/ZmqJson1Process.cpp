#include <iostream>

#include "ZmqHelper.hpp"
#include "ZmqSimpleLoadBalancer.hpp"
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
    JsonTypedSerializer<std::string> stringSerializer;

    zmq::context_t context{1};
    JsonTypedSerializer<std::string> strSer;
    JsonTypedDeserializer<std::string> strDes;
    ZmqRouter<std::string, std::string> testRouter{context, "tcp://127.0.0.1:5777", {strDes, strSer}};

    ZmqDealer<std::string, std::string> testDealer(context, "tcp://127.0.0.1:5777", {strSer, strDes});

    ZmqPoller poller{};

    /*ZmqRouter<std::string, std::string> test1{context, "tcp://127.0.0.1:1234", {strDes, strSer}};
    ZmqRouter<std::string, std::string> test2{context, "tcp://127.0.0.1:1235", {strDes, strSer}};*/
    using Checked = JsonTypedDeserializer<AuthenticatedOnPlanetRequest>&;
    using Requested = ITypedDeserializer<AuthenticatedOnPlanetRequest>&;
    static_assert(std::is_convertible<Checked, Requested>::value);

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

    std::string workStealingAddress = "tcp://127.0.0.1:1235";
    std::string workDealingAddress = "tcp://127.0.0.1:1234";
    std::string generalRequestAddress = "tcp://127.0.0.1:3333";
    std::string resourceOwnerAddress = "tcp://127.0.0.1:4444";

    ZmqRouter<AuthenticatedOnPlanetRequest, OnPlanetResponseNew> workDealerForBalancer{context, workDealingAddress, containerize(authPlanetRequestDeserializer, onPlanetResponsePlainSerializer)};
    ZmqRouter<WorkSharedPlanetRequest, AuthenticatedOnPlanetRequest> workStealerForBalancer{context, workStealingAddress, containerize(authPlanetRequestSerializer, workSharedPlanetRequestDeserializer)};

    ZmqSimpleLoadBalancer<AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> loadBalancer{workDealerForBalancer, workStealerForBalancer};

    ZmqWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew> logicWorkStealer{context, workStealingAddress, containerize(workStealerSerializer, onPlanetResponseNewCombinedSerializer, authPlanetRequestDeserializer)};

    sqlitedb::StorageDbFactory dbFactory{"testtest.db"};

    logger.debug("Loading configuration");

    Configuration configuration = loadConfiguration(CONFIGURATION_FILE_DIR "Configuration.json");

    auto db = dbFactory.create();

    ZmqDealer<LockRequestNew, LockResponseNew> lockRequestResOwnerDealer{context, resourceOwnerAddress, containerize(lockRequestSerializer, lockResponseDeserializer)};

    LogicApp logicApp{logicWorkStealer, lockRequestResOwnerDealer, *db, configuration};

    ZmqRouter<InternalGeneralRequest, InternalGeneralResponse> generalRequestRouter{context, generalRequestAddress, containerize(generalRequestDeserializer, generalResponseSerializer)};

    AccountServiceApp accServiceApp{*db, generalRequestRouter};

    Time realTime{};

    ZmqRouter<LockRequestNew, LockResponseNew> lockRequestResOwnerRouter{context, resourceOwnerAddress, containerize(lockRequestDeserializer, lockResponseSerializer)};

    ResourceOwnerApp resourceOwnerApp{realTime, lockRequestResOwnerRouter};

    logicWorkStealer.registerPoller(poller);
    generalRequestRouter.registerPoller(poller);
    lockRequestResOwnerRouter.registerPoller(poller);
    lockRequestResOwnerDealer.registerPoller(poller);
    workDealerForBalancer.registerPoller(poller);
    workStealerForBalancer.registerPoller(poller);
    poller.poll();
}