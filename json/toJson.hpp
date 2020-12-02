#pragma once
#include <boost/hana/for_each.hpp>
#include "nlohmann/json.hpp"
#include <iostream>
#include "BigNum.hpp"
#include "PlanetLocation.hpp"
#include "Timestamp.hpp"

using nlohmann::json;

namespace nlohmann
{
template<>
struct adl_serializer<Timestamp> {
    static inline void to_json(json &j, const Timestamp &timestamp) {
        j = timestamp.time_since_epoch().count();
    }
};

template<>
struct adl_serializer<Duration> {
    static inline void from_json(const json& j, Duration& duration) {
        duration = Duration{static_cast<int>(j)};
    }
};
}

template<typename T>
inline void to_json(json& j, const T& data, std::enable_if_t<boost::hana::Struct<T>::value>* = nullptr)
{
    using namespace boost;
    hana::for_each(hana::accessors<T>(), [&](auto accessor){
        j[hana::first(accessor).c_str()] = hana::second(accessor)(data);
    });
}


template<typename T>
constexpr void from_json(const json& j, T& data)
{
    using namespace boost;
    hana::for_each(hana::accessors<T>(), [&](auto accessor){
        auto fieldName = hana::first(accessor).c_str();
        if(not j.contains(fieldName))
        {
            std::cerr << j.dump() << " does not contain " << fieldName << std::endl;
            throw "no elo";
        }
        hana::second(accessor)(data) = j[fieldName];
    });
}

inline void to_json(json& j, const BigNum& num)
{
    j = num.toString();
}

template<typename T>
inline void to_json(json& j, const std::optional<T>& opt)
{
    if(opt)
    {
        j = *opt;
    }
    else
    {
        j = nullptr;
    }
}