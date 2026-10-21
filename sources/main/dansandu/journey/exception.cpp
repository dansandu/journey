#include "dansandu/journey/exception.hpp"

using dansandu::journey::utility::format;
using dansandu::journey::utility::wformat;

namespace dansandu::journey::exception
{

std::string formatException(const std::string& exception, const std::string& message, std::stacktrace stacktrace)
{
    return format("'", exception, "' exception in thread '", std::this_thread::get_id(), "': ", message, "\n",
                  stacktrace);
}

std::wstring wformatException(const std::string& exception, const std::wstring& message, std::stacktrace stacktrace)
{
    return wformat("'", exception, "' exception in thread '", std::this_thread::get_id(), "': ", message, "\n",
                   std::to_string(stacktrace));
}

}
