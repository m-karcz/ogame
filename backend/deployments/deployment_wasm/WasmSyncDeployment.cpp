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
#include "LoadConfiguration.hpp"

struct Processor
{
    inMemory::StorageDbFactory dbFactory{};
    Configuration configuration = loadConfiguration("/Configuration.json");
    RnDTime rndTime{};
    Time time;
    ITime& ttime = configuration.realTime ? (ITime&)time : (ITime&)rndTime;
    JsonSerializer serializer{};
    std::shared_ptr<IStorageDb> db = dbFactory.create();
    Service service{*db, ttime, configuration};
    RnDService rndService{ttime};
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

Processor processor;



using namespace emscripten;

EMSCRIPTEN_BINDINGS(elo){
    function("processRequest", +[](int ptr)->int{return processor.processRequest(ptr);});
    function("forwardTime", +[](int seconds){processor.ttime.shiftTimeBy(Duration{seconds});});
}

int main()
{
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