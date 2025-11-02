#pragma once

#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"

#include <functional>
#include <memory>
#include <string_view>

namespace dansandu::journey::logging
{

class PRALINE_EXPORT Logger
{
public:
    static Logger& globalInstance();

    explicit Logger(const std::wstring_view name);

    void addReporter(const std::wstring_view name, const Level level, std::function<void(const LogEntry&)> consumer);

    void removeReporter(const std::wstring_view name);

    void addChildLogger(Logger logger);

    void removeChildLogger(const std::wstring_view name);

    void setLevel(const Level level);

    Level getLevel() const;

    Level getHighestLevelLogged() const;

    void setRelativeFilePathFilter(const std::string_view filter);

    std::string getRelativeFilePathFilter() const;

    const std::wstring& getName() const;

    void log(const Level level, const char* const function, const char* const file, const int line, const int column,
             const char* const sourcesRoot, const std::wstring_view message);

private:
    void log(const LogEntry& logEntry);

    std::shared_ptr<void> implementation_;
};

}

#define LOG_DEBUG(...)                                                                                                 \
    dansandu::journey::logging::Logger::globalInstance().log(dansandu::journey::Level::debug, __func__, __FILE__,      \
                                                             __LINE__, 0, PRALINE_SOURCES_ROOT,                        \
                                                             dansandu::journey::utility::wformat(__VA_ARGS__));

#define LOG_INFO(...)                                                                                                  \
    dansandu::journey::logging::Logger::globalInstance().log(dansandu::journey::Level::info, __func__, __FILE__,       \
                                                             __LINE__, 0, PRALINE_SOURCES_ROOT,                        \
                                                             dansandu::journey::utility::wformat(__VA_ARGS__));

#define LOG_WARNING(...)                                                                                               \
    dansandu::journey::logging::Logger::globalInstance().log(dansandu::journey::Level::warning, __func__, __FILE__,    \
                                                             __LINE__, 0, PRALINE_SOURCES_ROOT,                        \
                                                             dansandu::journey::utility::wformat(__VA_ARGS__));

#define LOG_ERROR(...)                                                                                                 \
    dansandu::journey::logging::Logger::globalInstance().log(dansandu::journey::Level::error, __func__, __FILE__,      \
                                                             __LINE__, 0, PRALINE_SOURCES_ROOT,                        \
                                                             dansandu::journey::utility::wformat(__VA_ARGS__));

#define LOG_CRITICAL(...)                                                                                              \
    dansandu::journey::logging::Logger::globalInstance().log(dansandu::journey::Level::critical, __func__, __FILE__,   \
                                                             __LINE__, 0, PRALINE_SOURCES_ROOT,                        \
                                                             dansandu::journey::utility::wformat(__VA_ARGS__));
