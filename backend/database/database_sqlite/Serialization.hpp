#pragma once

#include <type_traits>
#include <string>
#include "Duration.hpp"
#include "Timestamp.hpp"
#include "BigNum.hpp"
#include "SingleEntity.hpp"

template<typename T>
struct Serialization
{};

template<>
struct Serialization<int>
{
    constexpr static const char* type = "INTEGER";
    static int deserialize(const char* repr)
    {
        return std::stoi(repr);
    }
    static std::string serialize(int value)
    {
        return std::to_string(value);
    }
};

template<>
struct Serialization<std::string>
{
    constexpr static const char* type = "TEXT";
    static std::string deserialize(const char* repr)
    {
        return repr;
    }
    static std::string serialize(std::string value)
    {
        return "\"" + value + "\"";
    }
};

template<>
struct Serialization<Duration> 
{
    constexpr static const char* type = Serialization<int>::type;
    static Duration deserialize(const char* repr)
    {
        return Duration{Serialization<int>::deserialize(repr)};
    }
    static std::string serialize(const Duration& value)
    {
        return Serialization<int>::serialize(value.count());
    }
};

template<>
struct Serialization<Timestamp>
{
    constexpr static const char* type = Serialization<Duration>::type;
    static Timestamp deserialize(const char* repr)
    {
        return Timestamp{Serialization<Duration>::deserialize(repr)};
    }
    static std::string serialize(const Timestamp& value)
    {
        return Serialization<Duration>::serialize(value.time_since_epoch());
    }
};

template<>
struct Serialization<BigNum>
{
    constexpr static const char* type = Serialization<std::string>::type;
    static BigNum deserialize(const char* repr)
    {
        return BigNum{std::stold(repr)};
    }
    static std::string serialize(const BigNum& value)
    {
        return Serialization<std::string>::serialize(value.toString());
    }
};

template<typename T>
struct Serialization<SingleEntity<T>>
{
    constexpr static const char* type = Serialization<std::string>::type;
    static SingleEntity<T> deserialize(const char* repr)
    {
        return SingleEntity<T>{repr};
    }
    static std::string serialize(const SingleEntity<T>& entity)
    {
        return Serialization<std::string>::serialize(entity.fieldName);
    }
};

template<typename T>
const char* serializationRepresentation()
{
    return Serialization<std::decay_t<T>>::type;
}