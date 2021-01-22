#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <ostream>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include "Logger.hpp"

constexpr size_t FRACTIONAL_SIZE = 6;
constexpr char COMMA_SEPARATOR = '.';

constexpr uint16_t MODULO_VALUE = 10000;

struct BigNum
{
    long double val = 0.0;
    BigNum(long double val) : val{val}
    {
    }
    BigNum() = default;
    BigNum(const BigNum&) = default;
    inline static BigNum fromStr(std::string_view str)
    {
        return BigNum{std::stold(std::string{str})};
    }
    std::string toString() const
    {
        std::stringstream ss;
        ss << std::fixed;
        ss.precision(5);
        ss << val;
        return ss.str();
    }
};

inline BigNum operator+(const BigNum& lhs, const BigNum& rhs)
{
    return {lhs.val + rhs.val};
}

inline BigNum operator-(const BigNum& lhs, const BigNum& rhs)
{
    return {lhs.val - rhs.val};
}

inline BigNum operator*(const BigNum& lhs, const BigNum& rhs)
{
    return {lhs.val * rhs.val};
}

inline BigNum operator/(const BigNum& lhs, const BigNum& rhs)
{
    return {lhs.val / rhs.val};
}

inline bool operator<(const BigNum& lhs, const BigNum& rhs)
{
    return lhs.val < rhs.val;
}

inline BigNum operator>>(const BigNum& lhs, unsigned rhs)
{
    return rhs > 0 ? (BigNum{lhs.val / 10.0} >> (rhs - 1)) : lhs;
}

inline BigNum operator<<(const BigNum& lhs, unsigned rhs)
{
    return rhs > 0 ? (BigNum{lhs.val * 10.0} << (rhs - 1)) : lhs;
}

inline BigNum pow(const BigNum& lhs, unsigned rhs)
{
    return BigNum{std::pow(lhs.val, (int)rhs)};
}
