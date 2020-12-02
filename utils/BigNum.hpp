#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <ostream>
#include <type_traits>


/*struct BigNum
{
    BigNum() : integral("0"), franctional('0', FRANCTIONAL_SIZE)
    {}
    BigNum(std::string integral, std::string fractional) : integral(std::move(integral)), fractional(std::move(fractional))
    {}
    std::string integral;
    std::string fractional;
    static std::vector<char> toReversedVector(const std::string& str)
    {
        std::vector<char> rev{str.rbegin(), str.rend()};
        for(auto& i : rev)
        {
            rev -= '0';
        }
        return rev;
    }
    static std::string toStr(std::vector<char> vec)
    {
        for(auto& i : vec)
        {
            rev += '0';
        }
        return std::string{rev.rbegin(), r.rend()};
    }
    std::string toString()
    {
        return integral + "." + fractional;
    }

    BigNum fromString(const std::string& str)
    {
        auto comma = str.find('.');
        return BigInt{str.substr(0, comma), str.substr(comma+1)};
    }
    friend std::ostream& operator<<(std::ostream& os, const BigNum& num)
    {
        return os << num.toString();
    }

    std::string squash()
    {
        return integral + fractional;
    }
};

BigNum operator+(const BigNum& lhsRaw, const BigNum& rhsRaw)
{
    auto lhs = toReversedVector(lhsRaw.squash());
    auto rhs = toReversedVector(rhsRaw.squash());

    char carry = 0;
    for()
}*/

constexpr size_t FRACTIONAL_SIZE = 6;
constexpr char COMMA_SEPARATOR = '.';


struct BigNum
{
    BigNum() : data('0', FRACTIONAL_SIZE + 1)
    {}

    template<typename Integer>
    BigNum(Integer integer, std::enable_if_t<std::is_integral_v<Integer>>* = nullptr)
    {
        *this = fromStr(std::to_string(integer));
    }
    template<typename Float>
    BigNum(Float floatish, std::enable_if_t<std::is_floating_point_v<Float>>* = nullptr)
    {
        *this = fromStr(std::to_string(floatish));
    }
    std::vector<char> data;
    bool isMinus = false;

    inline static BigNum fromStr(std::string_view str)
    {
        bool isMinus = str[0] == '-';
        if(isMinus)
        {
            str = str.substr(1);
        }
        std::vector<char> temp;
        temp.reserve(str.size() + FRACTIONAL_SIZE);
        for(auto digit : str)
        {
            if(digit != COMMA_SEPARATOR)
            {
                temp.push_back(digit - '0');
            }
        }
        /*auto commaPos = std::min(str.find(COMMA_SEPARATOR), str.size());
        auto zeroesToAdd = FRACTIONAL_SIZE - (str.size() - commaPos);
        temp.resize(temp.size() + zeroesToAdd, 0);*/
        auto commaPos = str.find(COMMA_SEPARATOR);
        if(commaPos == std::string::npos)
        {
            temp.resize(temp.size() + FRACTIONAL_SIZE, 0);
        }
        else
        {
            constexpr int SIGNED_FRACT_SIZE = FRACTIONAL_SIZE;
            int dataSignedSize = temp.size() + 1;
            int commaFromEnd = str.size() - commaPos;
            temp.resize(dataSignedSize + FRACTIONAL_SIZE - commaFromEnd, 0);
        }
        std::reverse(temp.begin(), temp.end());
        while(temp.back() == 0 and temp.size() > FRACTIONAL_SIZE)
        {
            temp.pop_back();
        }
        BigNum ret;
        ret.isMinus = isMinus;
        ret.data = std::move(temp);
        return ret;
    }

    inline std::string toString() const
    {
        std::string str;
        str.reserve(data.size() + 1);
        for(size_t i = 0; i < FRACTIONAL_SIZE; i++)
        {
            str += data[i] + '0';
        }
        str += ".";
        for(size_t i = FRACTIONAL_SIZE; i < data.size(); i++)
        {
            str += data[i] + '0';
        }
        if(str.back() == '.')
        {
            str += '0';
        }
        if(isMinus)
        {
            str += "-";
        }
        std::reverse(str.begin(), str.end());
        return str;
    }
    inline char get(size_t index) const
    {
        return index < data.size() ? data[index] : 0;
    }
    inline BigNum operator-() const
    {
        BigNum copy = *this;
        copy.isMinus = not isMinus;
        return copy;
    }
};

inline BigNum abs(BigNum num)
{
    num.isMinus = false;
    return num;
}

inline bool operator==(const BigNum& lhs, const BigNum& rhs)
{
    return lhs.isMinus == rhs.isMinus and lhs.data == rhs.data;
}

inline bool operator<(const BigNum& lhs, const BigNum& rhs)
{
    if(lhs.isMinus)
    {
        if(rhs.isMinus)
        {
            return abs(rhs) < abs(lhs);
        }
        else
        {
            return true;
        }
    }
    else
    {
        if(rhs.isMinus)
        {
            return false;
        }
        else
        {
            return std::lexicographical_compare(lhs.data.begin(), lhs.data.end(),
                                                rhs.data.begin(), rhs.data.end());
        }
    }
}

inline BigNum absAdd(const BigNum& lhs, const BigNum& rhs)
{
    std::vector<char> sum;
    size_t maxLength = std::max(lhs.data.size(), rhs.data.size());
    sum.reserve(maxLength + 1);
    char carry = 0;
    for(size_t i = 0; i < maxLength; i++)
    {
        carry += lhs.get(i) + rhs.get(i);
        sum.push_back(carry % 10);
        carry /= 10;
    }
    while(carry > 0)
    {
        sum.push_back(carry % 10);
        carry /= 10;
    }
    BigNum ret;
    ret.data = std::move(sum);
    return ret;
}

inline BigNum absSub(BigNum lhs, const BigNum& rhs)
{
    if(lhs < rhs)
    {
        return absSub(rhs, lhs);
    }
    /*bool wasCarried = false;
    for()*/
    for(auto& i : lhs.data)
    {
        i += 9;
    }
    lhs.data.front()++;
    if(lhs.data.back() == 10)
    {
        lhs.data.pop_back();
    }
    std::vector<char> sub;
    size_t maxLength = std::max(lhs.data.size(), rhs.data.size());
    sub.reserve(maxLength + 1);
    char carry = 0;
    for(size_t i = 0; i < maxLength; i++)
    {
        carry += lhs.get(i) - rhs.get(i);
        sub.push_back(carry % 10);
        carry /= 10;
    }
    while(carry > 0)
    {
        sub.push_back(carry % 10);
        carry /= 10;
    }
    BigNum ret;
    ret.data = std::move(sub);
    return ret;
}

inline BigNum operator+(const BigNum& lhs, const BigNum& rhs)
{
    if(lhs.isMinus == rhs.isMinus)
    {
        auto ret = absAdd(lhs, rhs);
        ret.isMinus = lhs.isMinus;
        return ret;
    }
    if(lhs.isMinus)
    {
        return rhs + lhs;
    }
    //rhs.isMinus
    auto ret = absSub(lhs, abs(rhs));
    ret.isMinus = abs(lhs) < abs(rhs);
    return ret;
}

inline BigNum operator-(const BigNum& lhs, const BigNum& rhs)
{
    return lhs + (- rhs);
}

inline BigNum operator<<(const BigNum& lhs, unsigned shift)
{
    BigNum copy = lhs;
    for(unsigned i = 0; i < shift; i++)
    {
        copy.data.insert(copy.data.begin(), 0);
    }
    return copy;
}

inline BigNum operator>>(const BigNum& lhs, unsigned shift)
{
    BigNum copy = lhs;
    for(unsigned i = 0; i < shift; i++)
    {
        if(not copy.data.empty())
        {
            copy.data = {copy.data.begin() + 1, copy.data.end()};
        }
    }
    while(copy.data.size() < FRACTIONAL_SIZE + 1)
    {
        copy.data.push_back(0);
    }
    return copy;
}

inline BigNum mulByDigit(const BigNum& lhs, char digit)
{
    BigNum out;
    out.data.clear();
    char carry = 0;
    for(auto c : lhs.data)
    {
        carry += c * digit;
        out.data.push_back(carry % 10);
        carry /= 10;
    }
    while(carry > 0)
    {
        out.data.push_back(carry % 10);
        carry /= 10;
    }
    return out;
}

inline BigNum operator*(const BigNum& lhs, const BigNum& rhs)
{
    BigNum result{};
    for(size_t i = 0; i < rhs.data.size(); i++)
    {
        result = result + mulByDigit(lhs, rhs.data[i]) << i;
    }
    return result >> FRACTIONAL_SIZE;
}

inline BigNum operator""_big(unsigned long long value)
{
    return BigNum{value};
}

inline BigNum operator""_big(long double value)
{
    return BigNum{value};
}

inline BigNum pow(const BigNum& lhs, unsigned value)
{
    if(value == 0)
    {
        return 1;
    }
    if(value <= 1)
    {
        return lhs;
    }
    return lhs * pow(lhs, value - 1);
}

inline BigNum operator/(const BigNum& lhs, const BigNum& rhs)
{
    return BigNum{std::stold(lhs.toString()) / std::stold(rhs.toString())};
}

