#pragma once

#include "dansandu/journey/common.hpp"

#include <memory>
#include <string>

namespace dansandu::journey::reporter
{

PRALINE_EXPORT void standardOutputLogReporter(const LogEntry& logEntry);

class PRALINE_EXPORT LogFileReporter
{
public:
    explicit LogFileReporter(const std::string& filePath);

    LogFileReporter(const LogFileReporter& other) = default;

    LogFileReporter(LogFileReporter&& other) noexcept;

    LogFileReporter& operator=(const LogFileReporter& other) = default;

    LogFileReporter& operator=(LogFileReporter&& other) noexcept;

    void operator()(const LogEntry& logEntry) const;

private:
    std::shared_ptr<void> implementation_;
};

}
