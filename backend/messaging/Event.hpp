#pragma once

#include <memory>
#include "Payload.hpp"
#include <optional>
#include "ISerializer.hpp"

template<typename T>
struct IEventHolder
{
    virtual ~IEventHolder() = default;
    virtual const T& get() const = 0;
};

template<typename T>
struct Event
{
    const T& get() const
    {
        return value->get();
    }
    const T& operator*() const
    {
        return get();
    }
    const T* operator->() const
    {
        return &get();
    }
    std::shared_ptr<IEventHolder<T>> value;
};

template<typename T>
struct PlainEventHolder : IEventHolder<T>
{
    PlainEventHolder(T value) : data{std::move(value)}
    {}
    const T& get() const override
    {
        return data;
    }
    T data;
};

template<typename T>
struct SerializedEventHolder : IEventHolder<T>
{
    SerializedEventHolder(ITypedDeserializer<T>& deserializer, Payload data) : deserializer{deserializer}, data{data}
    {}
    const T& get() const override
    {
        if(not deserialized)
        {
            deserialized = deserializer.deserialize(data);
        }
        return *deserialized;
    }
    ITypedDeserializer<T>& deserializer;
    mutable std::optional<T> deserialized{};
    Payload data;
};

struct IResponseSender
{
    virtual ~IResponseSender() = default;
    virtual void send(Payload) = 0;
};

struct StdFunctionSender : IResponseSender
{
    StdFunctionSender(std::function<void(Payload)> func) : func{func}
    {}
    void send(Payload payload) override
    {
        func(payload);
    }
    std::function<void(Payload)> func;
};

/*template<typename RespT>
struct ResponseHandler
{
    ITypedSerializer<RespT>& serializer;
    std::shared_ptr<IResponseSender> sender;
};*/

template<typename RespT>
struct IResponseHandler
{
    virtual void handle(const RespT&);
    virtual ~IResponseHandler() = default;
};

template<typename RespT>
struct StdFunctionResponseHandler : IResponseHandler<RespT>
{
    StdFunctionResponseHandler(std::function<void(const RespT&)> handler) : handler{handler}
    {}
    void handle(const RespT& resp) override
    {
        handler(resp);
    }
    std::function<void(const RespT&)> handler;
};

template<typename RespT>
struct SerializingResponseHandler : IResponseHandler<RespT>
{
    SerializingResponseHandler(ITypedSerializer<RespT>& serializer, std::shared_ptr<IResponseSender> sender)
        : serializer{serializer}
        , sender{sender}
    {}
    ITypedSerializer<RespT>& serializer;
    std::shared_ptr<IResponseSender> sender;
    void handle(const RespT& resp) override
    {
        sender->send(serializer.serialize(resp));
    }
};

template<typename ReqT, typename RespT>
struct Request : Event<ReqT>
{
    Request(Event<ReqT>&& event, std::shared_ptr<IResponseHandler<RespT>> responseHandler) : Event<ReqT>{std::move(event)}, responseHandler{responseHandler}
    {}
    using Event<ReqT>::get;
    using Event<ReqT>::operator*;
    using Event<ReqT>::operator->;
    std::shared_ptr<IResponseHandler<RespT>> responseHandler;
    void respond(const RespT& msg)
    {
        responseHandler->handle(msg);
    }
};

using TypeInformation = std::type_info;

struct TypeErasedEvent
{
    template<typename T>
    TypeErasedEvent(const Event<T>& event)
    {
        typeInformation = typeid(T);
        data = std::make_shared<Event<T>>(event);
    }
    TypeInformation typeInformation;
    std::shared_ptr<void> data;

    template<typename T>
    const Event<T>& as()
    {
        return *std::static_pointer_cast<Event<T>>(data);
    }
};

struct IPoller
{
    virtual void poll() = 0;
    virtual ~IPoller() = default;
};

struct IAsync
{
    virtual void registerPoller(IPoller&) = 0;
    virtual ~IAsync() = default;
};

template<typename ReqT, typename ResT>
struct IAsyncResponser : IAsync
{
    virtual void registerRequestHandler(std::function<void(Request<ReqT,ResT>)>) = 0;
};

template<typename ReqT, typename ResT>
struct IAsyncRequester : IAsync
{
    virtual void sendRequest(const ReqT&) = 0;
    virtual void registerResponseHandler(std::function<void(Event<ResT>)>) = 0;
};

template<typename IndT>
struct IIndicatorSender
{
    virtual ~IIndicatorSender() = default;
    virtual void sendIndication(const IndT&) = 0;
};

template<typename IndT>
struct IAsyncIndicatorHandler : IAsync
{
    virtual void registerIndicationHandler(std::function<void(Event<IndT>)>) = 0;
};

template<typename StartT, typename ReqT, typename ResT>
struct IAsyncWorkStealer : IAsync
{
    virtual void sendStartIndication(const StartT&) = 0;
    virtual void registerWorkStealing(std::function<void(Request<ReqT, ResT>)>) = 0;
};