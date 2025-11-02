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

    const auto flush = true;

    if (logEntry.level < Level::warning)
    {
        writeToStandardError(message, flush);
    }
    else
    {
        writeToStandardOutput(message, flush);
    }
}

struct LogFileReporterImplementation
{
    explicit LogFileReporterImplementation(const char* const filePath)
        : logFile{filePath, std::ios_base::out | std::ios_base::app}
    {
    }

    std::wofstream logFile;
    std::mutex mutex;
};

LogFileReporter::LogFileReporter(const char* const filePath)
    : implementation_{std::make_shared<LogFileReporterImplementation>(filePath)}
{
}

void LogFileReporter::operator()(const LogEntry& logEntry) const
{
    const auto message = wformat(logEntry.timestamp, ' ', toString(logEntry.level), ' ', logEntry.threadId, ' ',
                                 logEntry.relativeFilePath, '(', logEntry.line, ") ", logEntry.message, '\n');

    const auto impl = static_cast<LogFileReporterImplementation*>(implementation_.get());

    const auto lock = std::lock_guard<std::mutex>{impl->mutex};

    impl->logFile << message;
    impl->logFile.flush();
}

}
