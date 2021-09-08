#pragma once

#include "BigNum.hpp"
#include "Storage.hpp"
#include "Cost.hpp"


inline bool hasEnoughToPay(const Cost& required, const Storage& storage)
{
    return not(storage.metal < required.metal)
       and not(storage.crystal < required.crystal)
       and not(storage.deuter < required.deuter);
}