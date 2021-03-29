#include <zmq.hpp>
#include <string>
#include "TimeForwardRequest.hpp"
#include "ClearDatabaseRequest.hpp"
#include "SerializableRequest.hpp"
#include "toJson.hpp"

RndRequest parseReq(int argc, char* argv[])
{
    if(argc < 2)
    {
        throw std::logic_error{"empty parameters"};
    }
    std::string type = argv[1];
    if(type == "forwardTime" and argc > 2)
    {
        return {TimeForwardRequest{.duration = Duration{std::stoi(argv[2])}}};
    }
    if(type == "clearDb")
    {
        return {ClearDatabaseRequest{}};
    }
    throw std::logic_error{"wrong parameters"};
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        return -1;
    }

    RndRequest req{parseReq(argc, argv)};

    SerializableRequest serializable{req};

    zmq::context_t context{1};

    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:5555");

    Json j;
    j = serializeFrom(serializable);

    socket.send(zmq::buffer(j.dump()), zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);

    return 0;
}