#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/utility.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using dansandu::journey::utility::getFileName;
using dansandu::journey::utility::getLocalDateTime;

namespace dansandu::journey::logging
{

struct Reporter
{
    std::wstring name;
    Level level;
    std::function<void(const LogEntry&)> callback;
};

struct LoggerImplementation
{
    explicit LoggerImplementation(std::wstring name)
        : level{Level::debug}, highestLevelLogged{Level::debug}, name{std::move(name)}
    {
    }

    Level level;
    Level highestLevelLogged;
    const std::wstring name;
    std::vector<Reporter> reporters;
    std::vector<Logger> children;
    std::mutex mutex;
};

Logger& Logger::globalInstance()
{
    static auto logger = Logger{L"global"};
    return logger;
}

Logger::Logger(std::wstring name) : implementation_{std::make_shared<LoggerImplementation>(std::move(name))}
{
}

void Logger::addReporter(std::wstring name, const Level level, std::function<void(const LogEntry&)> reporter)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (std::find_if(impl->reporters.cbegin(), impl->reporters.cend(),
                     [&name](const auto& r) { return r.name == name; }) == impl->reporters.cend())
    {
        impl->reporters.push_back({std::move(name), level, std::move(reporter)});
    }
    else
    {
        throw std::logic_error("a reporter with the same name is already registered");
    }
}

void Logger::removeReporter(const std::wstring_view name)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (const auto position = std::find_if(impl->reporters.cbegin(), impl->reporters.cend(),
                                           [name](const auto& r) { return r.name == name; });
        position != impl->reporters.cend())
    {
        impl->reporters.erase(position);
    }
}

void Logger::addChildLogger(const Logger& logger)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (std::find_if(impl->children.cbegin(), impl->children.cend(),
                     [&logger](const auto& l) { return l.getName() == logger.getName(); }) == impl->children.cend())
    {
        impl->children.push_back(logger);
    }
    else
    {
        throw std::logic_error("a child logger with the same name is already registered");
    }
}

void Logger::removeChildLogger(const std::wstring_view name)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (const auto position = std::find_if(impl->children.cbegin(), impl->children.cend(),
                                           [&name](const auto& l) { return l.getName() == name; });
        position != impl->children.cend())
    {
        impl->children.erase(position);
    }
}

void Logger::setLevel(const Level level)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    impl->level = level;
}

Level Logger::getLevel() const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    return impl->level;
}

Level Logger::getHighestLevelLogged() const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    return impl->highestLevelLogged;
}

const std::wstring& Logger::getName() const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    return impl->name;
}

void Logger::log(const Level level, const std::wstring_view message, const std::source_location location)
{
    const auto fileName = getFileName(location.file_name());

    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (level < impl->highestLevelLogged)
    {
        impl->highestLevelLogged = level;
    }

    if (level != Level::none && level <= impl->level)
    {
        const auto logEntry = LogEntry{.level = level,
                                       .function = location.function_name(),
                                       .file = fileName,
                                       .line = static_cast<int>(location.line()),
                                       .column = static_cast<int>(location.column()),
                                       .threadId = std::this_thread::get_id(),
                                       .timestamp = getLocalDateTime(),
                                       .message = message};

        for (const auto& repoter : impl->reporters)
        {
            if (logEntry.level <= repoter.level)
            {
                repoter.callback(logEntry);
            }
        }

        for (auto& child : impl->children)
        {
            child.log(logEntry);
        }
    }
}

void Logger::log(const LogEntry& logEntry)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (logEntry.level < impl->highestLevelLogged)
    {
        impl->highestLevelLogged = logEntry.level;
    }

    if (logEntry.level != Level::none && logEntry.level <= impl->level)
    {
        for (const auto& repoter : impl->reporters)
        {
            if (logEntry.level <= repoter.level)
            {
                repoter.callback(logEntry);
            }
        }

        for (auto& child : impl->children)
        {
            child.log(logEntry);
        }
    }
}

}
