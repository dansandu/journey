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
    static const Logger& getGlobalInstance();

    explicit Logger(const std::wstring_view name);

    Logger(const std::wstring_view name, const Level level);

    Logger(const Logger& other) = default;

    Logger(Logger&& other) noexcept;

    Logger& operator=(const Logger& other) = default;

    Logger& operator=(Logger&& other) noexcept;

    void addReporter(const std::wstring_view name, const Level level,
                     std::function<void(const LogEntry&)> consumer) const;

    void removeReporter(const std::wstring_view name) const;

    void addChildLogger(const Logger& logger) const;

    void removeChildLogger(const std::wstring_view name) const;

    void setLevel(const Level level) const;

    Level getLevel() const;

    Level getHighestLevelLogged() const;

    void setRelativeFilePathFilter(const std::string_view filter) const;

    std::string getRelativeFilePathFilter() const;

    const std::wstring& getName() const;

    void log(const Level level, const std::string_view function, const std::string_view file, const int line,
             const int column, const std::string_view sourcesRoot, const std::wstring_view message) const;

private:
    std::shared_ptr<void> implementation_;
};

}

#define LOG(level, ...)                                                                                                \
    ::dansandu::journey::logging::Logger::getGlobalInstance().log(level, __func__, __FILE__, __LINE__, 0,              \
                                                                  PRALINE_SOURCES_ROOT,                                \
                                                                  ::dansandu::journey::utility::wformat(__VA_ARGS__))

#define LOG_DEBUG(...) LOG(::dansandu::journey::Level::debug, __VA_ARGS__)

#define LOG_INFO(...) LOG(::dansandu::journey::Level::info, __VA_ARGS__)

#define LOG_WARNING(...) LOG(::dansandu::journey::Level::warning, __VA_ARGS__)

#define LOG_ERROR(...) LOG(::dansandu::journey::Level::error, __VA_ARGS__)

#define LOG_CRITICAL(...) LOG(::dansandu::journey::Level::critical, __VA_ARGS__)
