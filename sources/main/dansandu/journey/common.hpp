#pragma once

#include <string>

namespace dansandu::journey
{

enum class Level
{
    none,
    debug,
    info,
    warning,
    error,
    critical,
};

struct DateTime
{
    short second;
    short minute;
    short hour;
    short day;
    short month;
    int year;
    std::string utcOffset;
};

struct LogEntry
{
    Level level;
    int line;
    int column;
    DateTime timestamp;
    std::string threadId;
    std::string relativeFilePath;
    std::string function;
    std::wstring message;
};

PRALINE_EXPORT const char* toString(const Level level);

PRALINE_EXPORT const char* toStringWithConsoleHighlight(const Level level);

PRALINE_EXPORT DateTime getLocalDateTime();

}
