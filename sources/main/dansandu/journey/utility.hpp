#pragma once

#include <sstream>
#include <string>
#include <string_view>

namespace dansandu::journey::utility
{

template<typename... Arguments>
auto format(const Arguments&... arguments)
{
    auto stream = std::ostringstream{};
    (stream << ... << arguments);
    return stream.str();
}

template<typename... Arguments>
auto wformat(const Arguments&... arguments)
{
    auto stream = std::wostringstream{};
    auto streamWriter = [&]<typename T>(const T& argument)
    {
        if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
        {
            stream << argument.c_str();
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::string_view>)
        {
            const auto string = static_cast<std::string>(argument);
            stream << string.c_str();
        }
        else
        {
            stream << argument;
        }
    };
    (streamWriter(arguments), ...);
    return stream.str();
}

PRALINE_EXPORT std::string getLocalDateTime();

PRALINE_EXPORT void writeToStandardOutput(const std::string_view string, const bool flush = false);

PRALINE_EXPORT void writeToStandardOutput(const std::wstring_view string, const bool flush = false);

PRALINE_EXPORT void writeToStandardError(const std::string_view string, const bool flush = false);

PRALINE_EXPORT void writeToStandardError(const std::wstring_view string, const bool flush = false);

PRALINE_EXPORT std::wstring toWideString(const char* const string);

PRALINE_EXPORT std::wstring toWideString(const std::string& string);

PRALINE_EXPORT std::wstring toWideString(std::wstring string);

PRALINE_EXPORT std::string getFileName(const std::string_view path);

PRALINE_EXPORT bool isSubpath(const std::string_view path, const std::string_view rootPath);

PRALINE_EXPORT bool tryGetRelativePath(const std::string_view path, const std::string_view rootPath,
                                       std::string& relativePath);

PRALINE_EXPORT std::string replaceBackSlashes(const std::string_view path);

}
