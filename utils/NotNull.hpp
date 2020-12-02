#pragma once

template<typename Pointer>
struct NotNull
{
    NotNull(const Pointer& ptr) : ptr{ptr}
    {}

    auto& operator*() const
    {
        return ptr.operator*();
    }

    auto& operator*()
    {
        return ptr.operator*();
    }

    auto* operator->() const
    {
        return ptr.operator->();
    }

    auto* operator->()
    {
        return ptr.operator->();
    }

private:
    Pointer ptr;
};