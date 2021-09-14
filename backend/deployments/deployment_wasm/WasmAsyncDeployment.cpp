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
#include <iostream>
//#include "RnDZmqCommandReceiver.hpp"
#include "Logger.hpp"
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include "JsonContainerMaker.hpp"
#include "WorkLoadHelperFunctions.hpp"

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
            callback(outputSerializer.serialize(*event));
        });
    }
    void sendMessage(Payload payload) override
    {
        this->sendRequest(inputDeserializer.deserialize(payload));
    }
};

struct WasmDealerWrapper 
{
    WasmDealerWrapper(std::shared_ptr<IJsonWasmDealer> wasmDealer) : wasmDealer{wasmDealer}
    {}
    void registerCallback(emscripten::val callback)
    {
        wasmDealer->registerHandler([=](Payload payload){
            callback(std::string{payload.begin(), payload.end()});
        });
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
std::function<void()> dbClearer;



struct WasmInstance
{
    Configuration configuration = loadConfiguration("/Configuration.json");
    WasmContext context{};
    WasmPoller poller{};
    JsonContainerMaker containerMaker{};
    WasmRouter<AuthenticatedOnPlanetRequest, OnPlanetResponseNew> workDealerForBalancer{context, configuration.addresses.workDealing, containerMaker};
    WasmRouter<WorkSharedPlanetRequest, AuthenticatedOnPlanetRequest> workStealerForBalancer{context, configuration.addresses.workStealing, containerMaker};
    SimpleLoadBalancer<AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> loadBalancer{workDealerForBalancer, workStealerForBalancer};
    WasmWorkStealer<WorkStealerReady, AuthenticatedOnPlanetRequest, OnPlanetResponseNew, WorkSharedPlanetRequest> logicWorkStealer{context, configuration.addresses.workStealing, containerMaker};
    sqlitedb::StorageDbFactory dbFactory{"/sqlite/testtest.db"};
    std::shared_ptr<IStorageDb> db = [&](){
        dbFactory.cleanIfNeeded();
        return dbFactory.create();
    }();
    WasmDealer<LockRequestNew, LockResponseNew> lockRequestResOwnerDealer{context, configuration.addresses.resourceOwner, containerMaker};
    LogicApp logicApp{logicWorkStealer, lockRequestResOwnerDealer, *db, configuration};
    WasmRouter<InternalGeneralRequest, InternalGeneralResponse> generalRequestRouter{context, configuration.addresses.generalRequest, containerMaker};
    AccountServiceApp accServiceApp{*db, generalRequestRouter};
    Time realTime{};
    WasmRouter<LockRequestNew, LockResponseNew> lockRequestResOwnerRouter{context, configuration.addresses.resourceOwner, containerMaker};
    ResourceOwnerApp resourceOwnerApp{realTime, lockRequestResOwnerRouter};
    WasmRouter<std::string, std::string> rndRouter{context, *configuration.addresses.rndMainAddress, containerMaker};

    WasmInstance(emscripten::val onReady)
    {
        forwardTimeExecutor = [&](int s){
            realTime.shiftTimeBy(std::chrono::seconds{s});
        };

        generalRequestWrapper = [&](){
            auto jsonWasmDealer = std::make_shared<JsonWasmDealer<InternalGeneralRequest,InternalGeneralResponse>>(
                context,
                configuration.addresses.generalRequest,
                containerMaker
            );

            jsonWasmDealer->registerPoller(poller);
            return WasmDealerWrapper{jsonWasmDealer};
        };

        onPlanetWrapper = [&](){
            auto jsonWasmDealer = std::make_shared<JsonWasmDealer<AuthenticatedOnPlanetRequest,OnPlanetResponseNew>>(
                context,
                configuration.addresses.workDealing,
                containerMaker
            );

            jsonWasmDealer->registerPoller(poller);
            return WasmDealerWrapper{jsonWasmDealer};
        };

        dbClearer = [&](){
            db->simulateVersionBreak();
        };

        logicWorkStealer.registerPoller(poller);
        generalRequestRouter.registerPoller(poller);
        lockRequestResOwnerRouter.registerPoller(poller);
        lockRequestResOwnerDealer.registerPoller(poller);
        workDealerForBalancer.registerPoller(poller);
        workStealerForBalancer.registerPoller(poller);

        onReady();

        poller.poll();
    }
    ~WasmInstance()
    {
        std::cout << "instance died" << std::endl;
    }
};

std::unique_ptr<WasmInstance> wasmInstance;


void mainImpl(emscripten::val onReady)
{
    wasmInstance = std::make_unique<WasmInstance>(onReady);
};

void prepareFsAndStart()
{
    EM_ASM({
        Module.FileSystem = FS;
        FS.mkdir("/sqlite");
        FS.mount(IDBFS, {}, '/sqlite');
        FS.syncfs(true, function(err){
            if(err)
            {
                console.log(err);
            }
            Module.mainImpl(Module.toMainImpl);
        });
    });
}

EMSCRIPTEN_BINDINGS(no_elo2) {
    function("getGeneralRequestDealer", +[](){return generalRequestWrapper();});
    function("getOnPlanetDealer", +[](){return onPlanetWrapper();});
    function("forwardTime", +[](int s){forwardTimeExecutor(s);});
    function("clearDb", +[](){dbClearer();});
    function("mainImpl", mainImpl);
    function("prepareFsAndStart", prepareFsAndStart);
}

int main()
{
    emscripten_exit_with_live_runtime();
}