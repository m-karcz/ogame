#include "ZmqHelper.hpp"
#include "Event.hpp"
#include <string>

int main()
{
    zmq::context_t context{1};
    ISerializer serializer{};
    ZmqDealer<std::string, std::string> testDealer{context, "tcp://127.0.0.1:5777", serializer};
    ZmqPoller poller;

    testDealer.registerResponseHandler([](auto msg){std::cout << "received: " << *msg << std::endl;});
    testDealer.registerPoller(poller);

    testDealer.sendRequest("no elo");

    poller.poll();

}