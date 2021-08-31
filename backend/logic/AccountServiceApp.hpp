#pragma once

#include "Event.hpp"
#include "AccountService.hpp"

struct IStorageDb;
struct InternalGeneralRequest;
struct InternalGeneralResponse;

struct AccountServiceApp
{
    AccountServiceApp(IStorageDb&, IAsyncResponser<InternalGeneralRequest, InternalGeneralResponse>&);
    AccountService service;
};