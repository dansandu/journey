#include "dansandu/journey/exception.hpp"

#include <thread>

namespace dansandu::journey::exception
{

std::string formatException(const std::string& exception, const std::string& message, std::stacktrace stacktrace)
{
    return ::dansandu::journey::utility::format("'", exception, "' thrown in thread '", std::this_thread::get_id(),
                                                "': ", message, "\n", stacktrace);
}

std::wstring wformatException(const std::string& exception, const std::wstring& message, std::stacktrace stacktrace)
{
    return ::dansandu::journey::utility::wformat("'", exception, "' thrown in thread '", std::this_thread::get_id(),
                                                 "': ", message, "\n", std::to_string(stacktrace));
}

}
