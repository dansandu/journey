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
    explicit LoggerImplementation(const std::wstring_view name, const Level level)
        : name{name}, level{level}, highestLevelLogged{Level::none}
    {
    }

    void log(const LogEntry& logEntry)
    {
        const auto lock = std::lock_guard<std::mutex>{mutex};

        if (logEntry.level > highestLevelLogged)
        {
            highestLevelLogged = logEntry.level;
        }

        if (logEntry.level != Level::none && logEntry.level >= level &&
            isSubpath(logEntry.relativeFilePath, relativeFilePathFilter))
        {
            for (const auto& repoter : reporters)
            {
                if (logEntry.level >= repoter.level)
                {
                    repoter.consumer(logEntry);
                }
            }

            for (auto& child : children)
            {
                child->log(logEntry);
            }
        }
    }

    const std::wstring name;

    Level level;
    Level highestLevelLogged;
    std::string relativeFilePathFilter;
    std::vector<Reporter> reporters;
    std::vector<std::shared_ptr<LoggerImplementation>> children;
    std::mutex mutex;
};

const Logger& Logger::getGlobalInstance()
{
    static const auto logger = Logger{L"global"};
    return logger;
}

Logger::Logger(const std::wstring_view name) : Logger{name, defaultLoggingLevel}
{
}

Logger::Logger(const std::wstring_view name, const Level level)
    : implementation_{std::make_shared<LoggerImplementation>(name, level)}
{
}

Logger::Logger(Logger&& other) noexcept : implementation_{other.implementation_}
{
}

Logger& Logger::operator=(Logger&& other) noexcept
{
    implementation_ = other.implementation_;
    return *this;
}

void Logger::addReporter(const std::wstring_view name, const Level level,
                         std::function<void(const LogEntry&)> consumer) const
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

void Logger::removeReporter(const std::wstring_view name) const
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

void Logger::addChildLogger(const Logger& other) const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto otherImpl = static_cast<LoggerImplementation*>(other.implementation_.get());

    if (impl == otherImpl)
    {
        throw std::logic_error("Cannot add self as a child logger");
    }

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    const auto position = std::find_if(impl->children.cbegin(), impl->children.cend(), [&](const auto& child)
                                       { return child.get() == otherImpl || child->name == otherImpl->name; });

    if (position == impl->children.cend())
    {
        impl->children.push_back(std::static_pointer_cast<LoggerImplementation>(other.implementation_));
    }
    else
    {
        throw std::logic_error("A child logger with the same name is already registered");
    }
}

void Logger::removeChildLogger(const std::wstring_view name) const
{
    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    const auto position = std::find_if(impl->children.cbegin(), impl->children.cend(),
                                       [&](const auto& child) { return child->name == name; });

    if (position != impl->children.cend())
    {
        impl->children.erase(position);
    }
}

void Logger::setLevel(const Level level) const
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

void Logger::setRelativeFilePathFilter(const std::string_view filter) const
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

void Logger::log(const Level level, const std::string_view function, const std::string_view file, const int line,
                 const int column, const std::string_view sourcesRoot, const std::wstring_view message) const
{
    auto relativePath = std::string{};

    const auto isSubpath = tryGetRelativePath(file, sourcesRoot, relativePath);

    const auto logEntry = LogEntry{
        .level = level,
        .line = line,
        .column = column,
        .threadId = std::this_thread::get_id(),
        .timestamp = getLocalDateTime(),
        .relativeFilePath = isSubpath ? replaceBackSlashes(relativePath) : getFileName(file),
        .function = static_cast<std::string>(function),
        .message = static_cast<std::wstring>(message),
    };

    const auto impl = static_cast<LoggerImplementation*>(implementation_.get());

    impl->log(logEntry);
}

}
