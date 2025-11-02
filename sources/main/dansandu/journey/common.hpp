#pragma once

#include <string>
#include <thread>

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

struct LogEntry
{
    Level level;
    const char* function;
    int line;
    int column;
    std::thread::id threadId;
    std::string relativeFilePath;
    std::string timestamp;
    std::wstring message;
};

PRALINE_EXPORT const char* toString(const Level level);

PRALINE_EXPORT const char* toStringWithConsoleHighlight(const Level level);

}
