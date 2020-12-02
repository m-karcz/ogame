#pragma once

#include "BigNum.hpp"

struct Materials
{
    BigNum metal;
    BigNum crystal;
    BigNum deuter;
};

inline bool hasEnoughToPay(const Materials& required, const Storage& storage)
{
    return not(storage.metal < required.metal)
       and not(storage.crystal < required.crystal)
       and not(storage.deuter < required.deuter);
}

/*inline bool hasEnoughToPay(const Materials& required, const Storage& storage)
{
    return not(storage.metal < required.metal)
           and not(storage.crystal < required.crystal)
           and not(storage.deuter < required.deuter);
}*/
