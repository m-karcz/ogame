#include <iostream>

#include "WasmHelper.hpp"
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
//#include "RnDZmqCommandReceiver.hpp"
#include "Logger.hpp"
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

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

using namespace emscripten;

struct IJsonWasmDealer
{
    virtual void registerHandler(std::function<void(Payload)>) = 0;
    virtual void sendMessage(Payload) = 0;
    virtual ~IJsonWasmDealer() = default;
};

template<typename ReqT, typename ResT>
struct JsonWasmDealer : WasmDealer<ReqT, ResT>, IJsonWasmDealer
{
    using WasmDealer<ReqT,ResT>::WasmDealer;
    JsonTypedDeserializer<ReqT> inputDeserializer;
    JsonTypedSerializer<ResT> outputSerializer;
    void registerHandler(std::function<void(Payload)> callback) override
    {
        this->registerResponseHandler([=](auto event){
            Payload serialized = outputSerializer.serialize(*event);
            logger.debug("sending resposne back to js: {}", std::string{serialized.begin(), serialized.end()});
            callback(serialized);
        });
        logger.debug("registered callback");
    }
    void sendMessage(Payload payload) override
    {
        auto message = inputDeserializer.deserialize(payload);
        this->sendRequest(message);
    }
};

struct WasmDealerWrapper 
{
    WasmDealerWrapper(std::shared_ptr<IJsonWasmDealer> wasmDealer) : wasmDealer{wasmDealer}
    {}
    void registerCallback(emscripten::val callback)
    {
        wasmDealer->registerHandler([=](Payload payload){
            logger.debug("responding with payload");
            std::string str{payload.begin(), payload.end()};
            callback(str);
        });
        logger.debug("registered handler");
    }
    void sendMessage(std::string str)
    {
        wasmDealer->sendMessage({str.begin(), str.end()});
    }

    std::shared_ptr<IJsonWasmDealer> wasmDealer;
};

EMSCRIPTEN_BINDINGS(no_elo) {
    class_<WasmDealerWrapper>("WasmDealerWrapper")
        .function("registerCallback", &WasmDealerWrapper::registerCallback)
        .function("sendMessage", &WasmDealerWrapper::sendMessage)
        ;
}

std::function<WasmDealerWrapper()> generalRequestWrapper;
std::function<WasmDealerWrapper()> onPlanetWrapper;
std::function<void(int)> forwardTimeExecutor;

EMSCRIPTEN_BINDINGS(no_elo2) {
    function("getGeneralRequestDealer", +[](){return generalRequestWrapper();});
    function("getOnPlanetDealer", +[](){return onPlanetWrapper();});
    function("forwardTime", +[](int s){forwardTimeExecutor(s);});
}


int main()
{
    WasmContext context;

    WasmPoller poller{};

    JsonTypedDeserializer<InternalGeneralRequest> generalRequestDeserializer;
    JsonTypedSerializer<InternalGeneralRequest> generalRequestSerializer;
    JsonTypedSerializer<InternalGeneralResponse> generalResponseSerializer;
    JsonTypedDeserializer<InternalGeneralResponse> generalResponseDeserializer;
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

    Configuration configuration = loadConfiguration("/Configuration.json");

    WasmRouter<AuthenticatedOnPlanetRequest, OnPlanetResponseNew> workDealerForBalancer{context, configuration.addresses.workDealing, containerize(authPlanetRequestDeserializer, onPlanetResponsePlainSerializer)};
    WasmRouter<WorkSharedPlanetRequest, AuthenticatedOnPlanetRequest> workStealerForBalancer{context, configuration.addresses.workStealing, containerize(authPlanetRequestSerializer, workSharedPlanetRequestDeserializer)};

    SimpleLoadBalancer<AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> loadBalancer{workDealerForBalancer, workStealerForBalancer};

    WasmWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew> logicWorkStealer{context, configuration.addresses.workStealing, containerize(workStealerSerializer, onPlanetResponseNewCombinedSerializer, authPlanetRequestDeserializer)};

    sqlitedb::StorageDbFactory dbFactory{"testtest.db"};


    dbFactory.cleanIfNeeded();

    auto db = dbFactory.create();

    WasmDealer<LockRequestNew, LockResponseNew> lockRequestResOwnerDealer{context, configuration.addresses.resourceOwner, containerize(lockRequestSerializer, lockResponseDeserializer)};

    LogicApp logicApp{logicWorkStealer, lockRequestResOwnerDealer, *db, configuration};

    WasmRouter<InternalGeneralRequest, InternalGeneralResponse> generalRequestRouter{context, configuration.addresses.generalRequest, containerize(generalRequestDeserializer, generalResponseSerializer)};

    AccountServiceApp accServiceApp{*db, generalRequestRouter};

    Time realTime{};

    forwardTimeExecutor = [&](int s){
        realTime.shiftTimeBy(std::chrono::seconds{s});
    };

    WasmRouter<LockRequestNew, LockResponseNew> lockRequestResOwnerRouter{context, configuration.addresses.resourceOwner, containerize(lockRequestDeserializer, lockResponseSerializer)};

    ResourceOwnerApp resourceOwnerApp{realTime, lockRequestResOwnerRouter};

    JsonTypedDeserializer<std::string> stringDeserializer;
    JsonTypedSerializer<std::string> stringSerializer;

    WasmRouter<std::string, std::string> rndRouter{context, *configuration.addresses.rndMainAddress, containerize(stringSerializer, stringDeserializer)};

    //RnDZmqCommandReceiver rnd{configuration, rndRouter, realTime, *db};

    generalRequestWrapper = [&](){
        auto jsonWasmDealer = std::make_shared<JsonWasmDealer<InternalGeneralRequest,InternalGeneralResponse>>(
            context,
            configuration.addresses.generalRequest,
            containerize(generalRequestSerializer, generalResponseDeserializer)
        );

        jsonWasmDealer->registerPoller(poller);
        return WasmDealerWrapper{jsonWasmDealer};
    };

    JsonTypedDeserializer<OnPlanetResponseNew> onPlanetResponseNewDeserializer;
    JsonTypedSerializer<AuthenticatedOnPlanetRequest> authenticatedOnPlanetRequestSerializer;

    onPlanetWrapper = [&](){
        auto jsonWasmDealer = std::make_shared<JsonWasmDealer<AuthenticatedOnPlanetRequest,OnPlanetResponseNew>>(
            context,
            configuration.addresses.workDealing,
            containerize(authenticatedOnPlanetRequestSerializer, onPlanetResponseNewDeserializer)
        );

        jsonWasmDealer->registerPoller(poller);
        return WasmDealerWrapper{jsonWasmDealer};
    };


    //rndRouter.registerPoller(poller);
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