#pragma once

#include <tuple>

namespace sqlite
{
template<auto> auto columnOptions = std::make_tuple();
}