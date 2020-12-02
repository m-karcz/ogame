#pragma once
#include <boost/hana/transform.hpp>
#include <sqlite_orm.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/concat.hpp>
#include "Constraints.hpp"
#include "ColumnOptions.hpp"

template<typename T, typename U>
U parent(T U::*memPtr);

template<auto memFunPtr>
auto getName()
{
    using namespace boost;
    using Parent = decltype(parent(memFunPtr));
    using Value = boost::hana::struct_detail::member_ptr<decltype(memFunPtr), memFunPtr>;
    constexpr auto isThatMember = hana::compose(hana::equal.to(hana::type_c<Value>), hana::typeid_, hana::second);
    return hana::first(hana::find_if(hana::accessors<Parent>(), isThatMember).value()).c_str();
}

template<typename T, T memFunPtr>
constexpr T getMemFunPtr(boost::hana::struct_detail::member_ptr<T, memFunPtr>)
{
    return memFunPtr;
}

template<typename Derived, typename T, typename Base>
T Derived::* upCast(T Base::*ptr)
{
    return ptr;
}

template<auto memFunPtr, typename...Args>
auto namedColumn(Args&&... args)
{
    using namespace sqlite_orm;
    return make_column(getName<memFunPtr>(), memFunPtr, std::forward<Args>(args)...);
}

template<typename Wrapper>
auto getBaseColumns()
{
    using namespace boost;
    using namespace sqlite_orm;

    return hana::transform(hana::accessors<typename Wrapper::Base>(), [](auto pair){
        constexpr auto memFunPtr = getMemFunPtr(hana::second(pair));

        return std::apply([&](auto &&... columnOpts) {
            return make_column(hana::first(pair).c_str(), upCast<Wrapper>(memFunPtr), columnOpts...);
        }, sqlite::columnOptions<memFunPtr>);
    });
}

template<typename Wrapper>
auto getWrapperColumns()
{
    using namespace boost;
    using namespace sqlite_orm;

    return hana::transform(hana::accessors<Wrapper>(), [](auto pair) {
        constexpr auto memFunPtr = getMemFunPtr(hana::second(pair));
        return std::apply([&](auto &&... columnOpts) {
            return make_column(hana::first(pair).c_str(), memFunPtr, columnOpts...);
            }, sqlite::columnOptions<memFunPtr>);
        });
}


template<typename Wrapper, typename...Rest>
auto makeTable(std::string name, Rest&&... rest)
{
    using namespace boost;
    using namespace sqlite_orm;
    return hana::unpack(hana::concat(getBaseColumns<Wrapper>(), getWrapperColumns<Wrapper>()), [&](auto&&... columns)
    {
        return std::apply([&](auto&&... constrs) {
            return make_table(name,
                              std::forward<decltype(columns)>(columns)...,
                              constrs...,
                              std::forward<Rest>(rest)...);
        }, sqlite::constraints<Wrapper>());
    });
}