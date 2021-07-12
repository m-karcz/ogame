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
    const T& get() const override
    {
        return data;
    }
    T data;
};

template<typename T>
struct SerializedEventHolder : IEventHolder<T>
{
    const T& get() const override
    {
        if(not deserialized)
        {
            deserialized = deserializer.deserialize(data);
        }
        return *deserialized;
    }
    ISerializer& deserializer;
    mutable std::optional<T> deserialized;
    Payload data;
};

struct IResponseSender
{
    virtual ~IResponseSender() = default;
    virtual void send(Payload) = 0;
};

template<typename T>
struct Request : Event<T>
{
    using Event<T>::get;
    using Event<T>::operator*;
    using Event<T>::operator->;
    ISerializer& serializer;
    std::shared_ptr<IResponseSender> sender;
    template<typename U>
    void respond(U&& msg)
    {}
};

