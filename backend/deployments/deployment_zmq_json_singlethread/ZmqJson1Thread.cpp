
#include <zmq.hpp>
#include "RnDTime.hpp"
#include "Logger.hpp"
#include "StorageDbFactory.hpp"
#include <iostream>
#include "Configuration.hpp"
#include "JsonSerializer.hpp"
#include "SingleInstance.hpp"

int main()
{
    logger.debug("Starting main");
    RnDTime rndTime;
    ITime& time = rndTime;
    inMemory::StorageDbFactory dbFactory;

    Configuration configuration{};

    JsonSerializer serializer;

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