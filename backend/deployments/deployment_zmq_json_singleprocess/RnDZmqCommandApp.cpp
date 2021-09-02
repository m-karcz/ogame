#include "LoadConfiguration.hpp"
#include "Configuration.hpp"
#include "JsonSerializer.hpp"
#include <zmq.hpp>
#include <sstream>


int main(int argc, char** argv)
{
    zmq::context_t context{1};
    const auto configuration = loadConfiguration(CONFIGURATION_FILE_DIR "Configuration.json");
    JsonTypedSerializer<std::string> serializer;

    std::stringstream ss;

    for(int i = 1; i < argc; i++)
    {
        ss << argv[i] << " ";
    }

    auto str = ss.str();
    str.pop_back();
    auto payload = serializer.serialize(str);

    zmq::socket_t socket{context, zmq::socket_type::req};

    socket.connect(*configuration.addresses.rndMainAddress);

    socket.send(zmq::buffer(std::move(payload)), zmq::send_flags::none);

    zmq::message_t msg{};
    (void)socket.recv(msg, zmq::recv_flags::none);

    return 0;
}