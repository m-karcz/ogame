#pragma once

#define ZMQ_BUILD_DRAFT_API 1
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include "Event.hpp"
#include <iostream>
#include <functional>

void sendMultipart(zmq::socket_ref socket, std::vector<zmq::message_t>& messages)
{
    zmq::send_multipart(socket, messages);
}

std::vector<zmq::message_t> recieveMultipart(zmq::socket_ref socket)
{
    std::vector<zmq::message_t> messages;
    (void)zmq::recv_multipart(socket, std::back_inserter(messages));
    return messages;
}
std::string hexDump( const zmq::message_t & aMessage ) {
  // I'm going to build a hex/ascii dump like you've seen so many times before
  std::string  msg;
  std::string  ascii;
  // get the pointer to the start of the message's payload - and it's size
  const char *buff = (const char *)aMessage.data();
  int         size = ((zmq::message_t &)aMessage).size();
  const char *end  = buff + size - 1;
  // see if it's the trivial case
  if (buff == NULL) {
    msg.append("NULL");
  } else {
    // get a place to hold the conversion of each byte
    char   hex[3];
    bzero(hex, 3);
    // run through the valid data in the buffer
    for (const char *p = buff; p <= end; ++p) {
      // generate the hex code for the byte and add it
      snprintf(hex, 3, "%02x", (uint8_t)(*p));
      msg.append(hex).append(" ");
      // if it's printable, add it to the ascii part, otherwise, put a '.'
      if (isprint(*p)) {
        ascii.append(p, 1);
      } else {
        ascii.append(".");
      }
      // see if we have a complete line
      if (ascii.size() >= 19) {
        msg.append(" ").append(ascii).append("\n");
        ascii.clear();
      }
    }
    // if we have anything left, put it on the line as well
    if (ascii.size() > 0) {
      msg.append((19 - ascii.length())*3 + 1, ' ').append(ascii);
    }
  }

  return msg;
}

    struct ZmqResponseSender : IResponseSender
    {
        ZmqResponseSender(std::vector<zmq::message_t> routing, zmq::socket_ref socket) : routingInfo{std::move(routing)}, socket{socket}
        {}
        std::vector<zmq::message_t> routingInfo;
        zmq::socket_ref socket;
        void send(Payload payload)
        {
            zmq::message_t msg{payload.data(), payload.size()};
            routingInfo.push_back(std::move(msg));
            sendMultipart(socket, routingInfo);
        }
    };

struct ZmqPoller : IPoller
{
    void poll() override
    {
        while(true)
        {
            native.wait(std::chrono::seconds(1));
        }
    }
    zmq::active_poller_t native;
};

template<typename ReqT, typename ResT>
struct ZmqRouter : IAsyncResponser<ReqT, ResT>
{
    ZmqRouter(zmq::context_t& context, std::string address, SerializationContainer<ITypedDeserializer<ReqT>&, ITypedSerializer<ResT>&> serializationContainer)
        : serializationContainer{serializationContainer}
        , socket{context, zmq::socket_type::router}
    {
        socket.bind(address);
    }
    void registerRequestHandler(std::function<void(Request<ReqT, ResT>)> handler) override
    {
        requestHandler = std::move(handler);
    }
    void registerPoller(IPoller& poller) override
    {
        auto& zmqPoller = dynamic_cast<ZmqPoller&>(poller).native;
        zmqPoller.add(socket, zmq::event_flags::pollin, [this](auto){
            requestHandler(readRequest(recieveMultipart(socket)));
        });
    }

private:
    Request<ReqT, ResT> readRequest(std::vector<zmq::message_t>&& rawRequest)
    {
        Event<ReqT> event{std::make_shared<SerializedEventHolder<ReqT>>(serializationContainer.template get<ITypedDeserializer<ReqT>&>(), Payload{(char*)rawRequest.back().data(), (char*)rawRequest.back().data() + rawRequest.back().size()})};
        rawRequest.pop_back();
        auto responseSender = std::make_shared<ZmqResponseSender>(std::move(rawRequest), socket);
        Request<ReqT, ResT> request{std::move(event), std::make_shared<SerializingResponseHandler<ResT>>(serializationContainer.template get<ITypedSerializer<ResT>&>(), responseSender)};
        return request;
    }

    SerializationContainer<ITypedDeserializer<ReqT>&, ITypedSerializer<ResT>&> serializationContainer;
    std::function<void(Request<ReqT, ResT>)> requestHandler;
    zmq::socket_t socket;
};

template<typename ReqT, typename ResT>
struct ZmqDealer : IAsyncRequester<ReqT, ResT>
{
    ZmqDealer(zmq::context_t& ctx, std::string address, SerializationContainer<ITypedSerializer<ReqT>&, ITypedDeserializer<ResT>&> serializationContainer)
        : serializationContainer{serializationContainer}
        , socket{ctx, zmq::socket_type::dealer}
    {
        socket.connect(address);
    }

    void sendRequest(const ReqT& request)
    {
        std::vector<zmq::message_t> msgs(1);
        Payload serialized = serializationContainer.template get<ITypedSerializer<ReqT>&>().serialize(request);
        //auto serialized = serializer.serialize(request);
        zmq::message_t msg{serialized.data(), serialized.size()};
        msgs.push_back(std::move(msg));
        sendMultipart(socket, msgs);
    }
    void registerResponseHandler(std::function<void(Event<ResT>)> handler) override
    {
        responseHandler = std::move(handler);
    }
    void registerPoller(IPoller& poller) override
    {
        auto& zmqPoller = dynamic_cast<ZmqPoller&>(poller).native;
        zmqPoller.add(socket, zmq::event_flags::pollin, [this](auto){
            responseHandler(readResponse(recieveMultipart(socket)));
        });
    }
    Event<ResT> readResponse(std::vector<zmq::message_t>&& rawResponse)
    {
        Event<ResT> event{std::make_shared<SerializedEventHolder<ResT>>(serializationContainer.template get<ITypedDeserializer<ResT>&>(), Payload{(char*)rawResponse.back().data(), (char*)rawResponse.back().data() + rawResponse.back().size()})};
        return std::move(event);
    }

    SerializationContainer<ITypedSerializer<ReqT>&, ITypedDeserializer<ResT>&> serializationContainer;
    std::function<void(Event<ResT>)> responseHandler;
    zmq::socket_t socket;
};

template<typename StartT, typename ReqT, typename ResT>
struct ZmqWorkStealer : IAsyncWorkStealer<StartT, ReqT, ResT>
{
    ZmqWorkStealer(zmq::context_t& context, std::string address, SerializationContainer<ITypedSerializer<StartT>&,
                                                                                        ITypedSerializer<ResT>&,
                                                                                        ITypedDeserializer<ReqT>&> serializationContainer)
        : socket{context, zmq::socket_type::dealer}
        , serializationContainer{serializationContainer}
    {
        socket.connect(address);
    }
    void sendStartIndication(const StartT& startReq) override
    {
        std::vector<zmq::message_t> msgs(1);
        //auto serialized = serializer.serialize(startReq);
        auto serialized = serializationContainer.template get<ITypedSerializer<StartT>&>().serialize(startReq);
        zmq::message_t msg{serialized.data(), serialized.size()};
        msgs.push_back(std::move(msg));
        sendMultipart(socket, msgs);
    }
    void registerWorkStealing(std::function<void(Request<ReqT, ResT>)> handler) override
    {
        workHandler = std::move(handler);
    }
    void registerPoller(IPoller& poller) override
    {
        auto& zmqPoller = dynamic_cast<ZmqPoller&>(poller).native;
        zmqPoller.add(socket, zmq::event_flags::pollin, [this](auto){
            workHandler(readWork(recieveMultipart(socket)));
        });
    }
    Request<ReqT, ResT> readWork(std::vector<zmq::message_t>&& rawRequest)
    {
        Event<ReqT> event{std::make_shared<SerializedEventHolder<ReqT>>(serializationContainer.template get<ITypedDeserializer<ReqT>&>(), Payload{(char*)rawRequest.back().data(), (char*)rawRequest.back().data() + rawRequest.back().size()})};
        rawRequest.pop_back();
        auto responseSender = std::make_shared<ZmqResponseSender>(std::move(rawRequest), socket);
        Request<ReqT, ResT> request{std::move(event), std::make_shared<SerializingResponseHandler<ResT>>(serializationContainer.template get<ITypedSerializer<ResT>&>(), responseSender)};
        return request;
    }

    std::function<void(Request<ReqT, ResT>)> workHandler;
    zmq::socket_t socket;
    SerializationContainer<ITypedSerializer<StartT>&,
                            ITypedSerializer<ResT>&,
                            ITypedDeserializer<ReqT>&> serializationContainer;
};