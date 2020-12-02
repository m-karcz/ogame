#include <boost/hana/for_each.hpp>
#include <boost/hana/accessors.hpp>
#include <unordered_map>
#include <string_view>
#include <string.h>

namespace detailConversion
{
    template<typename T, T memFunPtr>
    constexpr T getMemFunPtr(boost::hana::struct_detail::member_ptr<T, memFunPtr>) {
        return memFunPtr;
    }
    template<typename Ret, typename Class>
    Class getClassOfField(Ret Class::*);
    template<typename Field>
    using ClassOfField = decltype(getClassOfField(std::declval<Field>()));
}

template<typename Pack>
auto getPair(Pack pack)
{
    return std::make_pair(detailConversion::getMemFunPtr(boost::hana::second(pack)), std::string_view{boost::hana::first(pack).c_str()});
}

struct FieldHasher
{
    template<typename T>
    auto operator()(T field) const
    {
        static_assert(sizeof(void*) == sizeof(field));
        void* no_elo;
        memcpy(&no_elo, &field, sizeof(field));
        return std::hash<void*>{}(no_elo);
    }
};

template<typename Field>
auto generateField2StrMap()
{
    std::unordered_map<Field, std::string_view, FieldHasher> map;
    boost::hana::for_each(boost::hana::accessors<detailConversion::ClassOfField<Field>>(), [&](auto&& pack) {
        map.emplace(detailConversion::getMemFunPtr(boost::hana::second(pack)),
                    std::string_view{boost::hana::first(pack).c_str()});
    });
    return map;
}

template<typename Field>
auto generateStr2FieldMap()
{
    std::unordered_map<std::string_view, Field> map;
    boost::hana::for_each(boost::hana::accessors<detailConversion::ClassOfField<Field>>(), [&](auto&& pack) {
        map.emplace(std::string_view{boost::hana::first(pack).c_str()},
                    detailConversion::getMemFunPtr(boost::hana::second(pack)));
    });
    return map;
}

template<typename Field>
std::string_view field2str(Field field)
{
    static auto map = generateField2StrMap<Field>();
    auto it = map.find(field);
    if(it == map.end())
    {
        throw "no elo";
    }
    return it->second;
}
template<typename Field>
Field str2field(std::string_view str)
{
    static auto map = generateStr2FieldMap<Field>();
    auto it = map.find(str);
    if(it == map.end())
    {
        throw "no elo";
    }
    return it->second;
}
