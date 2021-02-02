#include <zmq.hpp>
#include <string>
#include "api/TimeForwardRequest.hpp"
#include "api/RnDRequestVariant.hpp"
#include "toJson.hpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        return -1;
    }
    int timeInMilliseconds = std::stoi(argv[1]);

    TimeForwardRequest timeReq{.duration = Duration{timeInMilliseconds}};

    RnDRequestVariant req{timeReq};

    zmq::context_t context{1};

    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:5555");

    Json j;
    j["data"] = serializeFrom(req);
    j["type"] = "rnd";

    socket.send(zmq::buffer(j.dump()), zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);

    return 0;
}