#pragma once

#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"

#include <functional>
#include <memory>
#include <source_location>

namespace dansandu::journey::logging
{

class PRALINE_EXPORT Logger
{
public:
    static Logger& globalInstance();

    explicit Logger(std::wstring name);

    void addReporter(std::wstring name, const Level level, std::function<void(const LogEntry&)> reporter);

    void removeReporter(const std::wstring_view name);

    void addChildLogger(const Logger& logger);

    void removeChildLogger(const std::wstring_view name);

    void setLevel(const Level level);

    Level getLevel() const;

    Level getHighestLevelLogged() const;

    const std::wstring& getName() const;

    void log(const Level level, const std::wstring_view message,
             const std::source_location location = std::source_location::current());

private:
    void log(const LogEntry& logEntry);

    std::shared_ptr<void> implementation_;
};

template<typename... Arguments>
struct LogCritical
{
    explicit LogCritical(const Arguments&... arguments,
                         const std::source_location location = std::source_location::current())
    {
        Logger::globalInstance().log(Level::critical, dansandu::journey::utility::wformat(arguments...), location);
    }
};

template<typename... Arguments>
LogCritical(const Arguments&...) -> LogCritical<Arguments...>;

template<typename... Arguments>
struct LogError
{
    explicit LogError(const Arguments&... arguments,
                      const std::source_location location = std::source_location::current())
    {
        Logger::globalInstance().log(Level::error, dansandu::journey::utility::wformat(arguments...), location);
    }
};

template<typename... Arguments>
LogError(const Arguments&...) -> LogError<Arguments...>;

template<typename... Arguments>
struct LogWarning
{
    explicit LogWarning(const Arguments&... arguments,
                        const std::source_location location = std::source_location::current())
    {
        Logger::globalInstance().log(Level::warning, dansandu::journey::utility::wformat(arguments...), location);
    }
};

template<typename... Arguments>
LogWarning(const Arguments&...) -> LogWarning<Arguments...>;

template<typename... Arguments>
struct LogInfo
{
    explicit LogInfo(const Arguments&... arguments,
                     const std::source_location location = std::source_location::current())
    {
        Logger::globalInstance().log(Level::info, dansandu::journey::utility::wformat(arguments...), location);
    }
};

template<typename... Arguments>
LogInfo(const Arguments&...) -> LogInfo<Arguments...>;

template<typename... Arguments>
struct LogDebug
{
    explicit LogDebug(const Arguments&... arguments,
                      const std::source_location location = std::source_location::current())
    {
        Logger::globalInstance().log(Level::debug, dansandu::journey::utility::wformat(arguments...), location);
    }
};

template<typename... Arguments>
LogDebug(const Arguments&...) -> LogDebug<Arguments...>;

}
