#pragma once

#define ZMQ_BUILD_DRAFT_API 1
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include "Event.hpp"
#include <iostream>
#include <functional>
#include "Logger.hpp"

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
        logger.debug("reading request {}", __PRETTY_FUNCTION__);
        Event<ReqT> event{std::make_shared<SerializedEventHolder<ReqT>>(serializationContainer.template get<ITypedDeserializer<ReqT>&>(), Payload{(char*)rawRequest.back().data(), (char*)rawRequest.back().data() + rawRequest.back().size()})};
        rawRequest.pop_back();
        auto responseSender = std::make_shared<ZmqResponseSender>(std::move(rawRequest), socket);
        Request<ReqT, ResT> request{std::move(event), std::make_shared<SerializingResponseHandler<ResT>>(serializationContainer.template get<ITypedSerializer<ResT>&>(), responseSender)};
        logger.debug("readed request {}", __PRETTY_FUNCTION__);
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

template<typename StartT, typename ReqT, typename ResT, typename CombinedT>
struct ZmqWorkStealer : IAsyncWorkStealer<StartT, ReqT, ResT>
{
    ZmqWorkStealer(zmq::context_t& context, std::string address, SerializationContainer<ITypedSerializer<CombinedT>&,
                                                                                        ITypedDeserializer<ReqT>&> serializationContainer)
        : socket{context, zmq::socket_type::dealer}
        , serializationContainer{serializationContainer}
    {
        socket.connect(address);
    }
    void sendStartIndication(const StartT& startReq) override
    {
        std::vector<zmq::message_t> msgs(1);
        auto serialized = serializationContainer.template get<ITypedSerializer<CombinedT>&>().serialize(wrapWorkerReady(startReq));
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
        auto& serializer = serializationContainer.template get<ITypedSerializer<CombinedT>&>();
        Request<ReqT, ResT> request(std::move(event), std::make_shared<TransformingResponseHandler<ResT, CombinedT>>(
                                                            [](auto&& resp){return wrapWorkResponse(resp);},
                                                            std::make_shared<SerializingResponseHandler<CombinedT>>(serializer, responseSender)));
        return request;
    }

    std::function<void(Request<ReqT, ResT>)> workHandler;
    zmq::socket_t socket;
    SerializationContainer<ITypedSerializer<CombinedT>&,
                           ITypedDeserializer<ReqT>&> serializationContainer;
};