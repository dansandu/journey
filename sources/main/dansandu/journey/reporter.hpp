#pragma once

#include "dansandu/journey/common.hpp"

#include <memory>

namespace dansandu::journey::reporter
{

PRALINE_EXPORT void standardOutputLogReporter(const LogEntry& logEntry);

class PRALINE_EXPORT LogFileReporter
{
public:
    explicit LogFileReporter(const char* const filePath);

    void operator()(const LogEntry& logEntry) const;

private:
    std::shared_ptr<void> implementation_;
};

}
