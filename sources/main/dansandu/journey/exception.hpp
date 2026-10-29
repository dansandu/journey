#pragma once

#include "dansandu/journey/utility.hpp"

#include <exception>
#include <stacktrace>
#include <string>

namespace dansandu::journey::exception
{

PRALINE_EXPORT std::string formatException(const std::string& exception, const std::string& message,
                                           std::stacktrace stacktrace = std::stacktrace::current());

PRALINE_EXPORT std::wstring wformatException(const std::string& exception, const std::wstring& message,
                                             std::stacktrace stacktrace = std::stacktrace::current());

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

    const char* what() const noexcept override
    {
        return "This is a wide string exception -- call getMessage() for the actual message";
    }

private:
    std::wstring message_;
};

#define THROW(ExceptionType, ...)                                                                                      \
    throw ExceptionType                                                                                                \
    {                                                                                                                  \
        ::dansandu::journey::exception::formatException(#ExceptionType,                                                \
                                                        ::dansandu::journey::utility::format(__VA_ARGS__))             \
    }

#define WTHROW(ExceptionType, ...)                                                                                     \
    throw ExceptionType                                                                                                \
    {                                                                                                                  \
        ::dansandu::journey::exception::wformatException(#ExceptionType,                                               \
                                                         ::dansandu::journey::utility::wformat(__VA_ARGS__))           \
    }

}
