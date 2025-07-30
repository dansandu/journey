#pragma once

#include <stdexcept>
#include <string>

namespace dansandu::journey::exception
{

class Exception : public std::exception
{
public:
    Exception()
    {
    }

    explicit Exception(const std::wstring& message) : message_{message}
    {
    }

    virtual const std::wstring& message() const noexcept
    {
        return message_;
    }

    const char* what() const noexcept override
    {
        return "This is a wide string exception -- call message() for the actual message";
    }

private:
    std::wstring message_;
};

}
