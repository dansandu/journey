#pragma once

#include <string>
#include <string_view>
#include <thread>

namespace dansandu::journey
{

enum class Level
{
    none,
    critical,
    error,
    warning,
    info,
    debug
};

struct LogEntry
{
    Level level;
    const char* function;
    const char* file;
    int line;
    int column;
    std::thread::id threadId;
    std::string timestamp;
    std::wstring_view message;
};

PRALINE_EXPORT const char* toString(const Level level);

PRALINE_EXPORT const char* toStringWithConsoleHighlight(const Level level);

}
