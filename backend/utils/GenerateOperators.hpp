#include <boost/hana/equal.hpp>
#include <boost/hana/type.hpp>

template<typename T>
constexpr bool SHOULD_GEN_EQUAL_OPERATOR = false;// boost::hana::is_valid([](auto&&){T::genEqOp;})(7);

template<typename T>
std::enable_if_t<SHOULD_GEN_EQUAL_OPERATOR<T>, bool> operator==(const T& lhs, const T& rhs)
{
    return boost::hana::equal(lhs, rhs);
}