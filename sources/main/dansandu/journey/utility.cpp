#include "dansandu/journey/utility.hpp"

#include <ctime>
#include <iostream>
#include <mutex>
#include <sstream>

namespace dansandu::journey::utility
{

static auto standardOutputMutex = std::mutex{};

std::string getLocalDateTime()
{
    auto timeInput = time_t{};
    time(&timeInput);

    auto timeOutput = tm{};

#ifdef _WIN32
    localtime_s(&timeOutput, &timeInput);
#else
    localtime_s(&timeInput, &timeOutput);
#endif

    char buffer[64];
    strftime(buffer, sizeof(buffer) / sizeof(*buffer), "%Y-%m-%d %H:%M:%S%z", &timeOutput);

    return buffer;
}

void writeToStandardOutput(const std::string_view string, const bool flush)
{
    const auto lock = std::lock_guard<std::mutex>{standardOutputMutex};
    std::cout << string;
    if (flush)
    {
        std::cout.flush();
    }
}

void writeToStandardOutput(const std::wstring_view string, const bool flush)
{
    const auto lock = std::lock_guard<std::mutex>{standardOutputMutex};
    std::wcout << string;
    if (flush)
    {
        std::wcout.flush();
    }
}

void writeToStandardError(const std::string_view string, const bool flush)
{
    const auto lock = std::lock_guard<std::mutex>{standardOutputMutex};
    std::cerr << string;
    if (flush)
    {
        std::cerr.flush();
    }
}

void writeToStandardError(const std::wstring_view string, const bool flush)
{
    const auto lock = std::lock_guard<std::mutex>{standardOutputMutex};
    std::wcerr << string;
    if (flush)
    {
        std::wcerr.flush();
    }
}

const char* getFileName(const char* filePath)
{
    if (!filePath)
    {
        return filePath;
    }

    auto fileName = filePath + std::strlen(filePath);
    while (fileName != filePath && *(fileName - 1) != '\\' && *(fileName - 1) != '/')
    {
        --fileName;
    }
    return fileName;
}

std::wstring toWideString(const char* const string)
{
    auto stream = std::wostringstream{};
    stream << string;
    return stream.str();
}

std::wstring toWideString(const std::string& string)
{
    return toWideString(string.c_str());
}

std::wstring toWideString(std::wstring string)
{
    return string;
}

}
