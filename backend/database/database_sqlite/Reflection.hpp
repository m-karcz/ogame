#pragma once
#include <boost/hana/for_each.hpp>
#include <boost/hana/accessors.hpp>

namespace sqlitedb
{
template<typename T, typename Callback>
void forEachField(T& obj, Callback&& callback)
{
    using namespace boost;
    hana::for_each(hana::accessors<T>(), [&](auto accessor){
        callback(hana::second(accessor)(obj));
    });
}

template<typename T, typename Callback>
void forEachFieldName(Callback&& callback)
{
    using namespace boost;
    hana::for_each(hana::accessors<T>(), [&](auto accessor){
        callback(hana::first(accessor).c_str());
    });
}

template<typename T, typename Callback>
void forEachNamedField(T& obj, Callback&& callback)
{
    using namespace boost;
    hana::for_each(hana::accessors<T>(), [&](auto accessor){
        callback(hana::first(accessor).c_str(), hana::second(accessor)(obj));
    });
}
}