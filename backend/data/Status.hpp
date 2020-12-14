#pragma once

#include <variant>
#include <memory>

struct IError
{
    virtual std::string serialize() = 0;
    virtual ~IError() = default;
};

struct Ok
{};

using Status = std::variant<Ok, std::unique_ptr<IError>>;

inline std::string serialize(const Status& status)
{
    if(std::get_if<Ok>(&status))
    {
        return "ok";
    }
    return std::get<std::unique_ptr<IError>>(status)->serialize();
}
