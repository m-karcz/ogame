#pragma once
#include <memory>
#include "Payload.hpp"
#include <functional>

template<typename T>
struct ITypedSerializer
{
    virtual ~ITypedSerializer() = default;
    virtual Payload serialize(const T&) const = 0;
};

template<typename T>
struct ITypedDeserializer
{
    virtual ~ITypedDeserializer() = default;
    virtual T deserialize(const Payload&) const = 0;
};

template<typename From, typename To>
struct TransformingSerializer : ITypedSerializer<From>
{
    TransformingSerializer(ITypedSerializer<To>& serializer, std::function<To(const From&)> transform)
        : serializer{serializer}
        , transform(std::move(transform))
    {}
    ITypedSerializer<To>& serializer;
    std::function<To(const From&)> transform;
    Payload serialize(const From& from) const override
    {
        return serializer.serialize(transform(from));
    }
};

template<typename...SerializationTypes>
struct SerializationContainer : std::tuple<SerializationTypes...>
{
    using std::tuple<SerializationTypes...>::tuple;
    template<typename...SerializationTypesWithOtherOrderOrItsSubrange>
    operator SerializationContainer<SerializationTypesWithOtherOrderOrItsSubrange...>() const
    {
        return {this->get<SerializationTypesWithOtherOrderOrItsSubrange>()...};
    }
    template<typename T>
    T get() const
    {
        return std::apply([](auto&... args)->T{
            return requestType<T, decltype(args)...>(args...);
        }, (const std::tuple<SerializationTypes...>&)(*this));
    }
    template<typename Requested, typename Checked, typename...Rest>
    static Requested requestType(Checked checked, Rest... rest)
    {
        if constexpr(std::is_convertible<Checked, Requested>::value)
        {
            return checked;
        }
        else
        {
            return requestType<Requested, Rest...>(rest...);
        }
    }
    template<typename Requested>
    static Requested requestType()
    {
        static_assert(not std::is_same_v<Requested, Requested>, "Tried to request not serviced type");
    }
};