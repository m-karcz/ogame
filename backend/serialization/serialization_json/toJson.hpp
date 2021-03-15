#pragma once
#include <boost/hana/for_each.hpp>
#include "nlohmann/json.hpp"
#include <iostream>
#include "BigNum.hpp"
#include "PlanetLocation.hpp"
#include "Timestamp.hpp"
#include "Json.hpp"
#include <variant>
#include "SingleEntityFwd.hpp"
#include "Status.hpp"
#include "GetTypeName.hpp"

namespace detail
{
    template<typename...T>
    constexpr auto getPack(std::variant<T...>)
    {
        return boost::hana::make_tuple(boost::hana::type_c<T>...);
    }

    template<typename Variant>
    constexpr auto pack = decltype(getPack(std::declval<Variant>())){};

    template<typename...T>
    constexpr auto getPack2()
    {
        return boost::hana::make_tuple(boost::hana::type_c<T>...);
    }
}


template<typename Instantiation, template<typename...> typename Template>
struct IsInstantiation
{
    template<typename T>
    struct Checker : std::false_type
    {

    };

    template<typename...T>
    struct Checker<Template<T...>> : std::true_type
    {

    };

    constexpr static bool value = Checker<Instantiation>::value;
};

template<typename Outer, template<typename...> typename Template>
struct FirstType
{
    template<typename...>
    struct Helper;

    template<typename T>
    struct Helper<Template<T>>
    {
        using Type = T;
    };

    /*template<typename T, typename...Rest>
    struct Helper<Template<T, Rest...>>
    {
        using Type = T;
    };*/


    using Type = typename Helper<Outer>::Type;
};

template<typename U>
Json serializeFrom(const U& obj)
{
    using T = std::decay_t<U>;
    if constexpr(std::is_same_v<T, Status>)
    {
        Json j;
        j = serialize(obj);
        return j;
    }
    else if constexpr(std::is_integral_v<T> or std::is_same_v<T, std::string>)
    {
        return obj;
    }
    else if constexpr(IsInstantiation<T, std::vector>::value)
    {
        Json j;
        for(auto& elem : obj)
        {
            j.push_back(serializeFrom(elem));
        }
        return j;
    }
    else if constexpr(IsInstantiation<T, std::variant>::value)
    {
        Json j;
        std::visit([&](auto&& elem){
            j["type"] = getTypeName<std::decay_t<decltype(elem)>>();
            j["data"] = serializeFrom(elem);
        }, obj);
        return j;
    }
    else if constexpr(boost::hana::Struct<T>::value)
    {
        using namespace boost;
        Json j;
        hana::for_each(hana::accessors<T>(), [&](auto accessor){
            j[hana::first(accessor).c_str()] = serializeFrom(hana::second(accessor)(obj));
        });
        return j;
    }
    else if constexpr(IsInstantiation<T, SingleEntity>::value)
    {
        return serializeFrom(obj.fieldName);
    }
    else if constexpr(IsInstantiation<T, std::optional>::value)
    {
        if(obj)
        {
            return serializeFrom(*obj);
        }
        else
        {
            return nullptr;
        }
    }
    else if constexpr(std::is_same_v<T, Timestamp>)
    {
        return serializeFrom(obj.time_since_epoch());
    }
    else if constexpr(std::is_same_v<T, Duration>)
    {
        return serializeFrom(obj.count());
    }
    else if constexpr(std::is_same_v<T, BigNum>)
    {
        auto str = obj.toString();
        return serializeFrom((int)std::stold(obj.toString()));
    }

    throw std::runtime_error{__PRETTY_FUNCTION__};
}


template<typename T>
T deserializeTo(const Json& j)
{
    logger.trace("Deserializing {} to {}", j.dump(), getTypeName<T>());
    if constexpr(std::is_integral_v<T> or std::is_same_v<T, std::string> or std::is_floating_point_v<T>)
    {
        return j;
    }
    else if constexpr(IsInstantiation<T, std::vector>::value)
    {
        using Inner = typename FirstType<T, std::vector>::Type;
        T vec;
        for(auto& elem : j)
        {
            vec.push_back(deserializeTo<Inner>(elem));
        }
        return vec;
    }
    else if constexpr(IsInstantiation<T, std::variant>::value)
    {
        using namespace boost;
        std::optional<T> deserialized;
        hana::for_each(::detail::pack<T>, [&](auto typeHolder){
            using t = typename decltype(typeHolder)::type;
            if(j["type"].get<std::string>() == getTypeName<std::decay_t<t>>())
            {
                try {
                    t attempt = deserializeTo<t>(j["data"]);
                    deserialized = attempt;
                }
                catch (...) {
                    throw;
                }
            }
        });
        return *deserialized;
    }
    else if constexpr(boost::hana::Struct<T>::value)
    {
        using namespace boost;
        T data;
        hana::for_each(hana::accessors<T>(), [&](auto accessor){
            auto fieldName = hana::first(accessor).c_str();
            using FieldType = std::decay_t<decltype(hana::second(accessor)(data))>;
            if(not j.contains(fieldName))
            {
                throw "no elo";
            }
            hana::second(accessor)(data) = deserializeTo<FieldType>(j[fieldName]);
        });
        return data;
    }
    else if constexpr(IsInstantiation<T, std::optional>::value)
    {
        if(j.is_null())
        {
            return std::nullopt;
        }
        else
        {
            return deserializeTo<typename FirstType<T, std::optional>::Type>(j);
        }
    }
    else if constexpr(IsInstantiation<T, SingleEntity>::value)
    {
        return {deserializeTo<std::string>(j)};
    }
    else if constexpr(std::is_same_v<T, Duration>)
    {
        return Duration{deserializeTo<int>(j)};
    }
    else if constexpr(std::is_same_v<T, BigNum>)
    {
        return BigNum{deserializeTo<double>(j)};
    }

    throw std::runtime_error(__PRETTY_FUNCTION__);
}