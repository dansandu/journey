#include "dansandu/journey/utility.hpp"

#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>

namespace dansandu::journey::utility
{

static auto standardOutputMutex = std::mutex{};

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

namespace
{

auto isSlash(const char c)
{
    return static_cast<bool>((c == '/') | (c == '\\'));
}

auto getCommonPathEndIndex(const std::string_view path, const std::string_view rootPath)
{
    auto pathPosition = path.cbegin();
    auto rootPosition = rootPath.cbegin();

    const auto areEqual = [](const char a, const char b)
    { return static_cast<bool>((a == b) | (isSlash(a) & isSlash(b))); };

    while (pathPosition != path.cend() && rootPosition != rootPath.cend() && areEqual(*pathPosition, *rootPosition))
    {
        ++pathPosition;
        ++rootPosition;
    }

    return pathPosition - path.cbegin();
}

bool tryGetRelativePathBeginIndex(const std::string_view path, const std::string_view rootPath, size_t& index)
{
    index = getCommonPathEndIndex(path, rootPath);

    if (rootPath.empty())
    {
        return true;
    }

    const auto rootPathEndsWithSlash = isSlash(rootPath.back());

    if (index == rootPath.size())
    {
        if (rootPathEndsWithSlash || index == path.size())
        {
            return true;
        }
        else if (index < path.size() && isSlash(path[index]))
        {
            ++index;
            return true;
        }
    }
    else if (index + 1 == rootPath.size() && rootPathEndsWithSlash && index == path.size())
    {
        return true;
    }

    return false;
}

}

std::string getFileName(const std::string_view path)
{
    auto position = path.cbegin() + path.size();

    while (position != path.cbegin() && !isSlash(*(position - 1)))
    {
        --position;
    }

    return std::string{position, path.cend()};
}

bool isSubpath(const std::string_view path, const std::string_view rootPath)
{
    auto index = size_t{};

    return tryGetRelativePathBeginIndex(path, rootPath, index);
}

bool tryGetRelativePath(const std::string_view path, const std::string_view rootPath, std::string& relativePath)
{
    auto index = size_t{};

    if (tryGetRelativePathBeginIndex(path, rootPath, index))
    {
        relativePath = std::string{path.cbegin() + index, path.cend()};
        return true;
    }

    return false;
}

std::string replaceBackSlashes(const std::string_view path)
{
    auto result = static_cast<std::string>(path);

    for (auto& character : result)
    {
        if (character == '\\')
        {
            character = '/';
        }
    }

    return result;
}

std::optional<std::string> getEnvironmentVariable(const std::string& variable)
{
#if defined(_WIN32)
    size_t requiredSize;

    getenv_s(&requiredSize, nullptr, 0, variable.c_str());
    if (requiredSize == 0)
    {
        return {};
    }

    const auto value = std::make_unique<char[]>(requiredSize);

    getenv_s(&requiredSize, value.get(), requiredSize, variable.c_str());

    return std::optional<std::string>{std::in_place, value.get()};
#elif defined(__linux__)
    const auto value = std::getenv(variable.c_str());
    if (value)
    {
        return std::optional<std::string>{std::in_place, value};
    }
    else
    {
        return {};
    }
#else
#error "Unknown platform"
#endif
}

}
