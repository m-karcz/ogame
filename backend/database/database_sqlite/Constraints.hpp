#pragma once

namespace sqlite
{
template<typename Wrapper> auto constraints()
{
    return std::make_tuple();
}
}