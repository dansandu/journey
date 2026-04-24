#pragma once

#include "dansandu/journey/utility.hpp"

#include <stacktrace>
#include <stdexcept>
#include <string>
#include <thread>

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

#define THROW(exception, ...)                                                                                          \
    throw exception{dansandu::journey::utility::format(                                                                \
        "'", #exception, "' exception in thread '", std::this_thread::get_id(),                                        \
        "': ", dansandu::journey::utility::format(__VA_ARGS__), "\n", std::stacktrace::current())};

#define WTHROW(exception, ...)                                                                                         \
    throw exception{dansandu::journey::utility::wformat(                                                               \
        "'", #exception, "' exception in thread '", std::this_thread::get_id(),                                        \
        "': ", dansandu::journey::utility::wformat(__VA_ARGS__), "\n", std::to_string(std::stacktrace::current()))};
