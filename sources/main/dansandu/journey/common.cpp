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
    case Level::debug:
        return "DEBUG";
    case Level::info:
        return "INFO";
    case Level::warning:
        return "WARNING";
    case Level::error:
        return "ERROR";
    case Level::critical:
        return "CRITICAL";
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
    case Level::debug:
        return "\x1B[34mDEBUG\x1B[0m";
    case Level::info:
        return "\x1B[32mINFO\x1B[0m";
    case Level::warning:
        return "\x1B[33mWARNING\x1B[0m";
    case Level::error:
        return "\x1B[31mERROR\x1B[0m";
    case Level::critical:
        return "\x1B[31mCRITICAL\x1B[0m";
    default:
        throw std::logic_error("Unknown logging level");
    }
}

}
