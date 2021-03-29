#include "RnDTime.hpp"
#include "Logger.hpp"
#include "StorageDbFactory.hpp"
#include <iostream>
#include "Configuration.hpp"
#include "Time.hpp"
#include "JsonSerializer.hpp"
#include "SingleInstance.hpp"
#include <emscripten.h>
#include <emscripten/bind.h>
#include "Logger.hpp"
#include "RndRequest.hpp"
#include "LoadConfiguration.hpp"

struct Processor
{
    sqlitedb::StorageDbFactory dbFactory{"/sqlite/testtest.db"};
    Configuration configuration = loadConfiguration("/Configuration.json");
    RnDTime rndTime{};
    Time time;
    ITime& ttime = configuration.realTime ? (ITime&)time : (ITime&)rndTime;
    JsonSerializer serializer{};
    int dbCheck = (dbFactory.cleanIfNeeded(), 0);
    std::shared_ptr<IStorageDb> db = dbFactory.create();
    Service service{*db, ttime, configuration};
    RnDService rndService{ttime, *db};
    SingleInstance instanceee{serializer, configuration, ttime, dbFactory};

int processRequest(int rawRequest)
{
    std::string request = {reinterpret_cast<const char*>(rawRequest)};
    free(reinterpret_cast<void*>(rawRequest));
        auto vecRet = instanceee.process({request.begin(), request.end()});
    std::string strRet{vecRet.begin(), vecRet.end()};
    void* rawRet = malloc(strRet.size() + 1);
    strcpy(reinterpret_cast<char*>(rawRet), strRet.data());
    return reinterpret_cast<int>(rawRet);
}
};

std::unique_ptr<Processor> processor;



using namespace emscripten;

EMSCRIPTEN_BINDINGS(elo){
    function("init", +[](void)->void{processor = std::make_unique<Processor>();});
    function("processRequest", +[](int ptr)->int{return processor->processRequest(ptr);});
    function("forwardTime", +[](int seconds){processor->ttime.shiftTimeBy(Duration{seconds});});
    //function("clearDb", +[](){processor->instanceee.storageDb->clearAndRecreate();});
    function("clearDb", +[](){processor->rndService.handleRequest(RndRequest{.request = ClearDatabaseRequest{}});
        EM_ASM({
            FS.syncfs(false, function(err){
                console.log(err);
                window.location.reload(true);
            });
        });
        /*EM_ASM({
            window.indexedDB.deleteDatabase("/sqlite");
            window.location.reload(true);
        });*/

        //EM_ASM({})
    
    });
}

int main()
{
    EM_ASM({
        Module.FileSystem = FS;
        FS.mkdir("/sqlite");
        FS.mount(IDBFS, {}, '/sqlite');
        FS.syncfs(true, function(err){
            console.log(err);
            Module.init();
        });
    });
    emscripten_set_main_loop(+[]{}, 0, 0);
}

/*EMSCRIPTEN_BINDINGS(elo){
    class_<Processor>("Processor")
    .constructor<>()
    .function("processRequest", &Processor::processRequest);
}

const auto test2 = [](){
    EM_ASM({console.log("another global initialized")});
    return 5;
}();

struct TestGlobal
{
    ~TestGlobal()
    {
        EM_ASM({console.log("global dead")});
    }
} globalDead;*/

/*int main()
{
    //logger.debug("no elo");
    return 0;
}*/

/*int main()
{
    EM_ASM({console.log("no elo")});
    emscripten_exit_with_live_runtime();
}*/