#pragma once

namespace sqlitedb
{
template<typename T>
struct PrimaryKeyed
{
    int id;
    T value;
};
}