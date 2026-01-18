#pragma once

#include <stdexcept>
#include <string>

namespace dansandu::journey::exception
{

class WideException : public std::exception
{
public:
    explicit WideException(const std::wstring& message) : message_{message}
    {
    }

    virtual const std::wstring& getMessage() const noexcept
    {
        return message_;
    }

    const char* what() const noexcept override final
    {
        return "This is a wide string exception -- call getMessage() for the actual message";
    }

private:
    std::wstring message_;
};

}
