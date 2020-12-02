#pragma once

#include "Json.hpp"
#include <variant>
#include "RequestVariant.hpp"
#include <boost/hana/for_each.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/core/demangle.hpp>
#include <variant>
#include "toJson.hpp"


namespace detail
{
    template<typename...T>
    constexpr auto getPack(std::variant<T...>)
    {
        return boost::hana::make_tuple(boost::hana::type_c<T>...);
    }

    template<typename Variant>
    constexpr auto pack = decltype(getPack(std::declval<Variant>())){};
}

template<typename Variant>
Variant deserializeRequest(const Json& json)
{
    namespace hana = boost::hana;
    std::optional<Variant> deserialized;
    if(json.contains("type") and json.contains("data"))
    {
        hana::for_each(detail::pack<Variant>, [&](auto typeHolder){
            using t = typename decltype(typeHolder)::type;
            if(json["type"] == boost::core::demangle(typeid(t).name()))
            {
                try {
                    t attempt;
                    from_json(json["data"], attempt);
                    deserialized = attempt;
                }
                catch (...) {}
            }
        });
    }
    if(not deserialized)
    {
        throw "no elo";
    }
    return *deserialized;
}

template<typename Variant>
std::vector<Variant> deserializeRequests(const Json& jsonList)
{
    std::vector<Variant> ret;
    for(auto req : jsonList)
    {
        ret.push_back(deserializeRequest<Variant>(req));
    }
    return ret;
}
