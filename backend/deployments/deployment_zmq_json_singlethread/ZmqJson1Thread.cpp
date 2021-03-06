#include <zmq.hpp>
#include "RnDTime.hpp"
#include "Logger.hpp"
#include "StorageDbFactory.hpp"
#include <iostream>
#include "Configuration.hpp"
#include "JsonSerializer.hpp"
#include "SingleInstance.hpp"
#include "LoadConfiguration.hpp"

int main()
{
    logger.debug("Starting main");
    RnDTime rndTime;
    ITime& time = rndTime;
    sqlitedb::StorageDbFactory dbFactory{"testtest.db"};

    logger.debug("Loading configuration");

    Configuration configuration = loadConfiguration(CONFIGURATION_FILE_DIR "Configuration.json");

    logger.debug("Loaded configuration");

    JsonSerializer serializer;

    logger.debug("Starting instance");

    dbFactory.cleanIfNeeded();

    SingleInstance instance{serializer, configuration, time, dbFactory};

    logger.debug("Started ZmqJson1Thread instance");

    zmq::context_t context{1};

    zmq::socket_t socket{context, zmq::socket_type::rep};
    socket.bind("tcp://*:5555");

    while(true)
    {
        zmq::message_t msg;
        socket.recv(msg, zmq::recv_flags::none);

        logger.debug("Received msg");

        std::vector<uint8_t> req{msg.data<uint8_t>(), msg.data<uint8_t>() + msg.size()};

        logger.debug("Making vec");

        auto resp = instance.process(req);

        logger.debug("Processed msg, sending response back");
        socket.send(zmq::buffer(resp.data(), resp.size()), zmq::send_flags::none);
    }
}