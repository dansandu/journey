#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/utility.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using dansandu::journey::utility::getFileName;
using dansandu::journey::utility::getLocalDateTime;
using dansandu::journey::utility::isSubpath;
using dansandu::journey::utility::replaceBackSlashes;
using dansandu::journey::utility::tryGetRelativePath;

namespace dansandu::journey::logging
{

namespace
{

#if defined(DEBUG) || defined(_DEBUG)
constexpr auto defaultLoggingLevel = Level::debug;
#else
constexpr auto defaultLoggingLevel = Level::info;
#endif

}

struct Reporter
{
    std::wstring name;
    Level level;
    std::function<void(const LogEntry&)> consumer;
};

struct LoggerImplementation
{
    explicit LoggerImplementation(const std::wstring_view name)
        : name{name}, level{defaultLoggingLevel}, highestLevelLogged{Level::none}
    {
    }

    const std::wstring name;

    Level level;
    Level highestLevelLogged;
    std::string relativeFilePathFilter;
    std::vector<Reporter> reporters;
    std::vector<Logger> children;
    std::mutex mutex;
};

Logger& Logger::globalInstance()
{
    static auto logger = Logger{L"global"};
    return logger;
}

Logger::Logger(const std::wstring_view name) : implementation_{std::make_shared<LoggerImplementation>(name)}
{
}

void Logger::addReporter(const std::wstring_view name, const Level level, std::function<void(const LogEntry&)> consumer)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    const auto position =
        std::find_if(impl->reporters.cbegin(), impl->reporters.cend(), [&](const auto& r) { return r.name == name; });

    if (position == impl->reporters.cend())
    {
        impl->reporters.push_back({static_cast<std::wstring>(name), level, std::move(consumer)});
    }
    else
    {
        throw std::logic_error("A reporter with the same name is already registered");
    }
}

void Logger::removeReporter(const std::wstring_view name)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    const auto position =
        std::find_if(impl->reporters.cbegin(), impl->reporters.cend(), [&](const auto& r) { return r.name == name; });

    if (position != impl->reporters.cend())
    {
        impl->reporters.erase(position);
    }
}

void Logger::addChildLogger(Logger logger)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (impl == logger.implementation_.get())
    {
        throw std::logic_error("Cannot add self as a child logger");
    }

    const auto position =
        std::find_if(impl->children.cbegin(), impl->children.cend(), [&](const auto& l)
                     { return l.getName() == logger.getName() || impl == logger.implementation_.get(); });

    if (position == impl->children.cend())
    {
        impl->children.push_back(logger);
    }
    else
    {
        throw std::logic_error("A child logger with the same name is already registered");
    }
}

void Logger::removeChildLogger(const std::wstring_view name)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    const auto position = std::find_if(impl->children.cbegin(), impl->children.cend(),
                                       [&](const auto& l) { return l.getName() == name; });

    if (position != impl->children.cend())
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

void Logger::setRelativeFilePathFilter(const std::string_view filter)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    impl->relativeFilePathFilter = filter;
}

std::string Logger::getRelativeFilePathFilter() const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    return impl->relativeFilePathFilter;
}

const std::wstring& Logger::getName() const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    return impl->name;
}

void Logger::log(const Level level, const char* const function, const char* const file, const int line,
                 const int column, const char* const sourcesRoot, const std::wstring_view message)
{
    auto relativePath = std::string{};

    const auto isSubpath = tryGetRelativePath(file, sourcesRoot, relativePath);

    const auto logEntry = LogEntry{.level = level,
                                   .function = function,
                                   .line = line,
                                   .column = column,
                                   .threadId = std::this_thread::get_id(),
                                   .relativeFilePath = isSubpath ? replaceBackSlashes(relativePath) : getFileName(file),
                                   .timestamp = getLocalDateTime(),
                                   .message = static_cast<std::wstring>(message)};

    log(logEntry);
}

void Logger::log(const LogEntry& logEntry)
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    if (logEntry.level > impl->highestLevelLogged)
    {
        impl->highestLevelLogged = logEntry.level;
    }

    if (logEntry.level != Level::none && logEntry.level >= impl->level &&
        isSubpath(logEntry.relativeFilePath, impl->relativeFilePathFilter))
    {
        for (const auto& repoter : impl->reporters)
        {
            if (logEntry.level >= repoter.level)
            {
                repoter.consumer(logEntry);
            }
        }

        for (auto& child : impl->children)
        {
            child.log(logEntry);
        }
    }
}

}
