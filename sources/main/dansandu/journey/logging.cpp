#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/exception.hpp"
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

using dansandu::journey::utility::format;
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
    std::string name;
    Level level;
    std::function<void(const LogEntry&)> consumer;
};

struct Logger::Implementation
{
    explicit Implementation(const std::string_view name, const Level level) : name{name}, level{level}
    {
    }

    void log(const LogEntry& logEntry) noexcept
    {
        const auto lock = std::lock_guard<std::mutex>{mutex};

        if (logEntry.level != Level::none && logEntry.level >= level &&
            isSubpath(logEntry.relativeFilePath, relativeFilePathFilter))
        {
            for (const auto& reporter : reporters)
            {
                if (logEntry.level >= reporter.level)
                {
                    reporter.consumer(logEntry);
                }
            }
        }
    }

    const std::string name;

    Level level;
    std::string relativeFilePathFilter;
    std::vector<Reporter> reporters;
    std::mutex mutex;
};

const Logger& Logger::getGlobalInstance()
{
    static const auto logger = Logger{"global"};
    return logger;
}

Logger::Logger(const std::string_view name) : Logger{name, defaultLoggingLevel}
{
}

Logger::Logger(const std::string_view name, const Level level)
    : implementation_{std::make_shared<Implementation>(name, level)}
{
}

void Logger::addReporter(const std::string_view name, const Level level,
                         std::function<void(const LogEntry&)> consumer) const
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    const auto position = std::find_if(implementation_->reporters.cbegin(), implementation_->reporters.cend(),
                                       [&](const auto& r) { return r.name == name; });

    if (position == implementation_->reporters.cend())
    {
        implementation_->reporters.push_back({static_cast<std::string>(name), level, std::move(consumer)});
    }
    else
    {
        THROW(std::logic_error, "A reporter with the same name is already registered");
    }
}

void Logger::removeReporter(const std::string_view name) const noexcept
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    const auto position = std::find_if(implementation_->reporters.cbegin(), implementation_->reporters.cend(),
                                       [&](const auto& r) { return r.name == name; });

    if (position != implementation_->reporters.cend())
    {
        implementation_->reporters.erase(position);
    }
}

void Logger::setLevel(const Level level) const noexcept
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    implementation_->level = level;
}

Level Logger::getLevel() const noexcept
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    return implementation_->level;
}

void Logger::setRelativeFilePathFilter(const std::string_view filter) const noexcept
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    implementation_->relativeFilePathFilter = filter;
}

std::string Logger::getRelativeFilePathFilter() const noexcept
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    return implementation_->relativeFilePathFilter;
}

const std::string& Logger::getName() const noexcept
{
    const auto lock = std::lock_guard<std::mutex>{implementation_->mutex};

    return implementation_->name;
}

void Logger::log(const Level level, const std::string_view function, const std::string_view file, const int line,
                 const std::string_view sourcesRoot, const std::string_view message) const noexcept
{
    auto relativePath = std::string{};

    const auto isSubpath = tryGetRelativePath(file, sourcesRoot, relativePath);

    const auto logEntry = LogEntry{
        .level = level,
        .line = line,
        .timestamp = getLocalDateTime(),
        .threadId = format(std::this_thread::get_id()),
        .relativeFilePath = isSubpath ? replaceBackSlashes(relativePath) : getFileName(file),
        .function = static_cast<std::string>(function),
        .message = static_cast<std::string>(message),
    };

    implementation_->log(logEntry);
}

}
