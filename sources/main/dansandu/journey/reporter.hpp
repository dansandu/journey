#pragma once

#include "dansandu/journey/common.hpp"

#include <memory>
#include <string>
#include <vector>

namespace dansandu::journey::reporter
{

PRALINE_EXPORT void standardOutputLogReporter(const LogEntry& logEntry);

class PRALINE_EXPORT LogFileReporter
{
public:
    explicit LogFileReporter(const std::string& filePath);

    LogFileReporter(const LogFileReporter& other) = default;

    LogFileReporter(LogFileReporter&& other) noexcept = default;

    LogFileReporter& operator=(const LogFileReporter& other) = default;

    LogFileReporter& operator=(LogFileReporter&& other) noexcept = default;

    void operator()(const LogEntry& logEntry) const;

private:
    std::shared_ptr<void> implementation_;
};

class PRALINE_EXPORT InMemoryReporter
{
public:
    InMemoryReporter();

    InMemoryReporter(const InMemoryReporter& other) = default;

    InMemoryReporter(InMemoryReporter&& other) noexcept = default;

    InMemoryReporter& operator=(const InMemoryReporter& other) = default;

    InMemoryReporter& operator=(InMemoryReporter&& other) noexcept = default;

    void operator()(const LogEntry& logEntry) const;

    void enable(const bool isEnabled) const;

    std::vector<LogEntry> getLoggedEntries() const;

private:
    std::shared_ptr<void> implementation_;
};

}
