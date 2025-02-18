#include "dansandu/journey/common.hpp"

#include <stdexcept>

namespace dansandu::journey
{

const char* toString(const Level level)
{
    switch (level)
    {
    case Level::none:
        return "NONE";
    case Level::critical:
        return "CRITICAL";
    case Level::error:
        return "ERROR";
    case Level::warning:
        return "WARNING";
    case Level::info:
        return "INFO";
    case Level::debug:
        return "DEBUG";
    default:
        throw std::logic_error("Unknown logging level");
    }
}

const char* toStringWithConsoleHighlight(const Level level)
{
    switch (level)
    {
    case Level::none:
        return "NONE";
    case Level::critical:
        return "\x1B[31mCRITICAL\x1B[0m";
    case Level::error:
        return "\x1B[31mERROR\x1B[0m";
    case Level::warning:
        return "\x1B[33mWARNING\x1B[0m";
    case Level::info:
        return "\x1B[34mINFO\x1B[0m";
    case Level::debug:
        return "\x1B[35mDEBUG\x1B[0m";
    default:
        throw std::logic_error("Unknown logging level");
    }
}

}
