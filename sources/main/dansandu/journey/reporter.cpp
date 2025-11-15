#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"

#include <fstream>
#include <memory>
#include <mutex>

using dansandu::journey::utility::getFileName;
using dansandu::journey::utility::wformat;
using dansandu::journey::utility::writeToStandardError;
using dansandu::journey::utility::writeToStandardOutput;

namespace dansandu::journey::reporter
{

void standardOutputLogReporter(const LogEntry& logEntry)
{
    const auto message =
        wformat(logEntry.timestamp, ' ', toStringWithConsoleHighlight(logEntry.level), ' ', logEntry.threadId, ' ',
                getFileName(logEntry.relativeFilePath), '(', logEntry.line, ") ", logEntry.message, '\n');

    if (logEntry.level == Level::none)
    {
        return;
    }

    const auto flush = true;

    if (logEntry.level <= Level::warning)
    {
        writeToStandardOutput(message, flush);
    }
    else
    {
        writeToStandardError(message, flush);
    }
}

struct LogFileReporterImplementation
{
    explicit LogFileReporterImplementation(const std::string& filePath)
        : logFile{filePath, std::ios_base::out | std::ios_base::app}
    {
    }

    std::wofstream logFile;
    std::mutex mutex;
};

LogFileReporter::LogFileReporter(const std::string& filePath)
    : implementation_{std::make_shared<LogFileReporterImplementation>(filePath)}
{
}

LogFileReporter::LogFileReporter(LogFileReporter&& other) noexcept : implementation_{other.implementation_}
{
}

LogFileReporter& LogFileReporter::operator=(LogFileReporter&& other) noexcept
{
    implementation_ = other.implementation_;
    return *this;
}

void LogFileReporter::operator()(const LogEntry& logEntry) const
{
    if (logEntry.level == Level::none)
    {
        return;
    }

    const auto message = wformat(logEntry.timestamp, ' ', toString(logEntry.level), ' ', logEntry.threadId, ' ',
                                 logEntry.relativeFilePath, '(', logEntry.line, ") ", logEntry.message, '\n');

    const auto impl = static_cast<LogFileReporterImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    impl->logFile << message;
    impl->logFile.flush();
}

}
