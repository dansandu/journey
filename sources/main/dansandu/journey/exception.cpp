#include "dansandu/journey/exception.hpp"

#include <thread>

using dansandu::journey::utility::format;
using dansandu::journey::utility::wformat;

namespace dansandu::journey::exception
{

std::string formatException(const std::string& exception, const std::string& message, std::stacktrace stacktrace)
{
    return format("'", exception, "' thrown in thread '", std::this_thread::get_id(), "': ", message, "\n", stacktrace);
}

std::wstring wformatException(const std::string& exception, const std::wstring& message, std::stacktrace stacktrace)
{
    return wformat("'", exception, "' thrown in thread '", std::this_thread::get_id(), "': ", message, "\n",
                   std::to_string(stacktrace));
}

}
