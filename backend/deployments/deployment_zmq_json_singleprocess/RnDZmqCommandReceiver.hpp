#pragma once

#include "Configuration.hpp"
#include "ZmqHelper.hpp"
#include "ITime.hpp"
#include "IStorageDb.hpp"
#include <regex>
#include <cstdio>
#include "Logger.hpp"

struct RnDZmqCommandReceiver
{
    RnDZmqCommandReceiver(const Configuration& conf,
                          ZmqRouter<std::string, std::string>& receiver,
                          ITime& time,
                          IStorageDb& storage)
        : conf{conf},
          receiver{receiver},
          time{time},
          storage{storage}
    {
        receiver.registerRequestHandler([this](Request<std::string, std::string> req){
            this->handle(std::move(req));
        });
    }
    void handle(Request<std::string, std::string> command)
    {
        int timeToForward;
        logger.debug("received command: {}", *command);
        if(*command == "clearDb")
        {
            logger.debug("simulating db break");
            storage.simulateVersionBreak();
        }
        else if(sscanf(command->c_str(), "forwardTime %i", &timeToForward) == 1)
        {
            logger.debug("shifting time by {} s" , timeToForward);
            time.shiftTimeBy(Duration{timeToForward});
        }

        command.respond("");
    }
    ITime& time;
    ZmqRouter<std::string, std::string>& receiver;
    const Configuration& conf;
    IStorageDb& storage;
};