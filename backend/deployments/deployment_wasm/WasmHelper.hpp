#pragma once

#include "Event.hpp"
#include <emscripten.h>
#include <queue>
#include "Logger.hpp"
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include "Logger.hpp"

struct WasmMessage
{
    std::optional<std::string> sourceAddress;
    Payload payload{};
};

struct WasmQueue
{
    std::queue<WasmMessage> messages{};
};

struct IPollable
{
    virtual void poll() = 0;
    virtual ~IPollable()
    {
        logger.debug("some pollable dead");
    }
};

struct WasmPoller : IPoller
{
    void poll() override;
    std::vector<IPollable*> pollables;
};

struct WasmContext
{
    std::map<std::string, WasmQueue> queues;
};

template<typename ReqT, typename ResT>
struct WasmDealer : IAsyncRequester<ReqT, ResT>, IPollable
{
    WasmDealer(WasmContext& ctx, std::string address, SerializationContainer<ITypedSerializer<ReqT>&, ITypedDeserializer<ResT>&> serializationContainer)
        : ctx{ctx},
          serializationContainer{serializationContainer},
          address{address}
    {
        logger.debug("created wasm dealer");
        ownGeneratedAddress = std::to_string((intptr_t)(this));
        ctx.queues[ownGeneratedAddress];
    }

    void registerResponseHandler(std::function<void(Event<ResT>)> handler) override
    {
        //responseHandler = std::move(handler);
        responseHandler = [=](auto event){
            logger.debug("received response from {}", address);
            handler(std::move(event));
        };
    }

    void registerPoller(IPoller& poller) override
    {
        dynamic_cast<WasmPoller&>(poller).pollables.push_back(this);
    }

    void sendRequest(const ReqT& request) override
    {
        Payload serialized = serializationContainer.template get<ITypedSerializer<ReqT>&>().serialize(request);

        ctx.queues[address].messages.push(WasmMessage{
            .sourceAddress = ownGeneratedAddress,
            .payload = serialized//std::move(serialized)
        });
        logger.debug("sent request {} to address {}", std::string{serialized.begin(), serialized.end()}, address);
    }

    void poll() override
    {
        auto& queue = ctx.queues[ownGeneratedAddress];
        if(not queue.messages.empty())
        {
            auto wasmMessage = queue.messages.front();
            queue.messages.pop();
            auto& deserializer = serializationContainer.template get<ITypedDeserializer<ResT>&>();
            Event<ResT> event{std::make_shared<SerializedEventHolder<ResT>>(deserializer, wasmMessage.payload)};
            responseHandler(std::move(event));
        }
    }

    WasmContext& ctx;
    SerializationContainer<ITypedSerializer<ReqT>&, ITypedDeserializer<ResT>&> serializationContainer;
    std::function<void(Event<ResT>)> responseHandler;
    std::string address;
    std::string ownGeneratedAddress;
};

struct WasmResponseSender : IResponseSender
{
    WasmResponseSender(std::string sourceAddress, WasmContext& ctx, std::optional<std::string> ownAddress = std::nullopt)
        : sourceAddress{std::move(sourceAddress)}
        , ctx{ctx}
        , ownAddress{ownAddress}
    {}
    std::string sourceAddress;
    std::optional<std::string> ownAddress;
    WasmContext& ctx;
    void send(Payload payload) override
    {
        logger.debug("sending response {} to {}", std::string{payload.begin(), payload.end()}, sourceAddress);
        ctx.queues[sourceAddress].messages.push(WasmMessage{
            .sourceAddress = ownAddress,
            .payload = std::move(payload)
        });
    }
};

template<typename ReqT, typename ResT>
struct WasmRouter : IAsyncResponser<ReqT, ResT>, IPollable
{
    WasmRouter(WasmContext& ctx, std::string address, SerializationContainer<ITypedDeserializer<ReqT>&, ITypedSerializer<ResT>&> serializationContainer)
        : ctx{ctx}
        , address{address}
        , serializationContainer{serializationContainer}
    {
        ctx.queues[address];
    }

    void registerRequestHandler(std::function<void(Request<ReqT, ResT>)> handler) override
    {
        requestHandler = std::move(handler);
    }

    void registerPoller(IPoller& poller) override
    {
        dynamic_cast<WasmPoller&>(poller).pollables.push_back(this);
    }

    void poll() override
    {
        auto& queue = ctx.queues[address];
        if(not queue.messages.empty())
        {
            auto wasmMessage = queue.messages.front();
            queue.messages.pop();
            auto& deserializer = serializationContainer.template get<ITypedDeserializer<ReqT>&>();
            Event<ReqT> event{std::make_shared<SerializedEventHolder<ReqT>>(deserializer, wasmMessage.payload)};
            auto responseSender = std::make_shared<WasmResponseSender>(*wasmMessage.sourceAddress, ctx);
            auto& serializer = serializationContainer.template get<ITypedSerializer<ResT>&>();
            Request<ReqT, ResT> request(std::move(event), std::make_shared<SerializingResponseHandler<ResT>>(serializer, responseSender));
            requestHandler(std::move(request));
        }
    }

    WasmContext& ctx;
    SerializationContainer<ITypedSerializer<ResT>&, ITypedDeserializer<ReqT>&> serializationContainer;
    std::function<void(Request<ReqT, ResT>)> requestHandler;
    std::string address;
};

void WasmPoller::poll() 
{
    emscripten_async_call(+[](void* self){
        auto* dis = static_cast<decltype(this)>(self);
        for(auto* pollable : dis->pollables)
        {
            pollable->poll();
        }
        dis->poll();
    }, (void*)this, 30);

}

template<typename StartT, typename ReqT, typename ResT, typename CombinedT>
struct WasmWorkStealer : IAsyncWorkStealer<StartT, ReqT, ResT>, IPollable
{
    WasmWorkStealer(WasmContext& context, std::string address, SerializationContainer<ITypedDeserializer<ReqT>&,
                                                                                        ITypedSerializer<CombinedT>&> serializationContainer)
        : ctx{context}
        , address{address}
        , serializationContainer{serializationContainer}
    {
        ownGeneratedAddress = std::to_string((intptr_t)(this));
    }
    void registerWorkStealing(std::function<void(Request<ReqT, ResT>)> handler) override
    {
        workHandler = std::move(handler);
    }
    void sendStartIndication(const StartT& startReq) override
    {
        send(serializationContainer.template get<ITypedSerializer<CombinedT>&>().serialize(wrapWorkerReady(startReq)));
    }
    void send(Payload payload)
    {
        ctx.queues[address].messages.push(WasmMessage{
            .sourceAddress = ownGeneratedAddress,
            .payload = std::move(payload)
        });
    }
    void registerPoller(IPoller& poller) override
    {
        dynamic_cast<WasmPoller&>(poller).pollables.push_back(this);
    }

    void poll() override
    {
        auto& queue = ctx.queues[ownGeneratedAddress];
        if(not queue.messages.empty())
        {
            auto wasmMessage = queue.messages.front();
            queue.messages.pop();
            Event<ReqT> event{std::make_shared<SerializedEventHolder<ReqT>>(serializationContainer.template get<ITypedDeserializer<ReqT>&>(), wasmMessage.payload)}; auto responseSender = std::make_shared<WasmResponseSender>(address, ctx, ownGeneratedAddress);
            auto& serializer = serializationContainer.template get<ITypedSerializer<CombinedT>&>();
            Request<ReqT, ResT> request(std::move(event), std::make_shared<TransformingResponseHandler<ResT, CombinedT>>(
                                                                [](auto&& resp){return wrapWorkResponse(resp);},
                                                                std::make_shared<SerializingResponseHandler<CombinedT>>(serializer, responseSender)));
            workHandler(std::move(request));
        }
    }

    WasmContext& ctx;
    std::string address;
    std::string ownGeneratedAddress;
    SerializationContainer<ITypedDeserializer<ReqT>&,
                           ITypedSerializer<CombinedT>&> serializationContainer;
    std::function<void(Request<ReqT, ResT>)> workHandler;
};