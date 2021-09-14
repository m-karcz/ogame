#pragma once

#include "JsonSerializer.hpp"
#include <unordered_map>
#include <memory>
#include <typeindex>


struct JsonContainerMaker
{
private:
    template<typename T>
    struct Tag{};
    template<typename T>
    ITypedSerializer<T>& get(Tag<ITypedSerializer<T>&> tag)
    {
        auto& ptr = storage[typeid(tag)];
        if(not ptr)
        {
            ptr = std::make_shared<JsonTypedSerializer<T>>();
        }
        return *std::static_pointer_cast<JsonTypedSerializer<T>>(ptr);
    }
    template<typename T>
    ITypedDeserializer<T>& get(Tag<ITypedDeserializer<T>&> tag)
    {
        auto& ptr = storage[typeid(tag)];
        if(not ptr)
        {
            ptr = std::make_shared<JsonTypedDeserializer<T>>();
        }
        return *std::static_pointer_cast<JsonTypedDeserializer<T>>(ptr);
    }
    std::unordered_map<std::type_index, std::shared_ptr<void>> storage;

public:
    template<typename...SerializersAndDeserializers>
    operator SerializationContainer<SerializersAndDeserializers...>()
    {
        return {
            get(Tag<SerializersAndDeserializers>{})...
        };
    }
};