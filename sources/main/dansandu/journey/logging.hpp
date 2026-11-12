#pragma once

#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace dansandu::journey::logging
{

class PRALINE_EXPORT Logger
{
public:
    static const Logger& getGlobalInstance();

    explicit Logger(const std::string_view name);

    Logger(const std::string_view name, const Level level);

    Logger(const Logger& other) = default;

    Logger(Logger&& other) noexcept = default;

    Logger& operator=(const Logger& other) = default;

    Logger& operator=(Logger&& other) noexcept = default;

    void addReporter(const std::string_view name, const Level level,
                     std::function<void(const LogEntry&)> consumer) const;

    void removeReporter(const std::string_view name) const noexcept;

    void setLevel(const Level level) const noexcept;

    Level getLevel() const noexcept;

    void setRelativeFilePathFilter(const std::string_view filter) const noexcept;

    std::string getRelativeFilePathFilter() const noexcept;

    const std::string& getName() const noexcept;

    void log(const Level level, const std::string_view function, const std::string_view file, const int line,
             const std::string_view sourcesRoot, const std::string_view message) const noexcept;

private:
    struct Implementation;

    std::shared_ptr<Implementation> implementation_;
};

}

#define LOG_WITH_LEVEL(level, ...)                                                                                     \
    ::dansandu::journey::logging::Logger::getGlobalInstance().log(                                                     \
        level, __func__, __FILE__, __LINE__, PRALINE_SOURCES_ROOT, ::dansandu::journey::utility::format(__VA_ARGS__))

#define LOG_DEBUG(...) LOG_WITH_LEVEL(::dansandu::journey::Level::debug, __VA_ARGS__)

#define LOG_INFO(...) LOG_WITH_LEVEL(::dansandu::journey::Level::info, __VA_ARGS__)

#define LOG_WARNING(...) LOG_WITH_LEVEL(::dansandu::journey::Level::warning, __VA_ARGS__)

#define LOG_ERROR(...) LOG_WITH_LEVEL(::dansandu::journey::Level::error, __VA_ARGS__)

#define LOG_CRITICAL(...) LOG_WITH_LEVEL(::dansandu::journey::Level::critical, __VA_ARGS__)
