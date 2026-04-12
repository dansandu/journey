#include "dansandu/journey/common.hpp"

#include <ctime>
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

DateTime getLocalDateTime()
{
    auto timeInput = time_t{};
    time(&timeInput);

    auto timeOutput = tm{};

#if defined(_WIN32)
    localtime_s(&timeOutput, &timeInput);
#elif defined(__linux__)
    localtime_r(&timeInput, &timeOutput);
#else
#error "Unknown platform"
#endif

    char utcOffset[32];
    strftime(utcOffset, std::size(utcOffset), "%z", &timeOutput);

    return DateTime{
        .second = static_cast<short>(timeOutput.tm_sec),
        .minute = static_cast<short>(timeOutput.tm_min),
        .hour = static_cast<short>(timeOutput.tm_hour),
        .day = static_cast<short>(timeOutput.tm_mday),
        .month = static_cast<short>(timeOutput.tm_mon + 1),
        .year = timeOutput.tm_year + 1900,
        .utcOffset = utcOffset,
    };
}

}
