#include <zmq.hpp>
#include <string>
#include "TimeForwardRequest.hpp"
#include "SerializableRequest.hpp"
#include "toJson.hpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        return -1;
    }
    int timeInMilliseconds = std::stoi(argv[1]);

    TimeForwardRequest timeReq{.duration = Duration{timeInMilliseconds}};

    RndRequest req{RndRequestData{timeReq}};

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