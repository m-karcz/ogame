#pragma once

template<typename T>
constexpr std::string_view getMagicString()
{
    return __PRETTY_FUNCTION__;
}

struct StructWithKnownName
{
};

template<typename T>
constexpr std::string_view getTypeName()
{
    constexpr auto knownMagic = getMagicString<StructWithKnownName>();
    constexpr std::string_view knownString = "StructWithKnownName";
    constexpr auto start = knownMagic.find(knownString);
    constexpr auto beforeEnd = knownMagic.size() - start - knownString.size();

    constexpr auto wantedMagic = getMagicString<T>();
    return wantedMagic.substr(start, wantedMagic.size() - beforeEnd - start);
}