#pragma once

#include "dansandu/journey/utility.hpp"

#include <exception>
#include <stacktrace>
#include <string>

namespace dansandu::journey::exception
{

class Exception : public std::exception
{
public:
    explicit Exception(const std::string& message) noexcept : message_{message}
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

PRALINE_EXPORT std::string formatException(const std::string& exception, const std::string& message,
                                           std::stacktrace stacktrace = std::stacktrace::current());

#define THROW(ExceptionType, ...)                                                                                      \
    throw ExceptionType                                                                                                \
    {                                                                                                                  \
        ::dansandu::journey::exception::formatException(#ExceptionType,                                                \
                                                        ::dansandu::journey::utility::format(__VA_ARGS__))             \
    }

}
