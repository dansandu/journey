#pragma once

#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace dansandu::journey::utility
{

template<typename T>
concept HasToStreamMethod =
    requires { static_cast<void (std::decay_t<T>::*)(std::ostream&) const>(&std::decay_t<T>::toStream); };

template<typename T>
concept HasToWideStreamMethod =
    requires { static_cast<void (std::decay_t<T>::*)(std::wostream&) const>(&std::decay_t<T>::toStream); };

template<typename T>
concept HasToStringMethod =
    requires { static_cast<std::string (std::decay_t<T>::*)() const>(&std::decay_t<T>::toString); };

template<typename T>
concept HasToWideStringMethod =
    requires { static_cast<std::wstring (std::decay_t<T>::*)() const>(&std::decay_t<T>::toWideString); };

template<typename T>
concept HasToStreamFunction = requires(std::ostream stream, const T value) { stream << value; };

template<typename T>
concept HasToWideStreamFunction = requires(std::wostream stream, const T value) { stream << value; };

template<typename... Arguments>
auto format(const Arguments&... arguments)
{
    auto stream = std::ostringstream{};

    const auto writer = [&]<typename T>(const T& argument)
    {
        if constexpr (HasToStreamMethod<T>)
        {
            argument.toStream(stream);
        }
        else if constexpr (HasToStreamFunction<T>)
        {
            stream << argument;
        }
        else if constexpr (HasToStringMethod<T>)
        {
            stream << argument.toString();
        }
        else
        {
            static_assert(!"Cannot format type");
        }
    };

    (writer(arguments), ...);

    return stream.str();
}

template<typename... Arguments>
auto wformat(const Arguments&... arguments)
{
    auto stream = std::wostringstream{};

    const auto writer = [&]<typename T>(const T& argument)
    {
        if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
        {
            stream << argument.c_str();
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::string_view>)
        {
            for (const auto c : argument)
            {
                stream << c;
            }
        }
        else if constexpr (HasToWideStreamMethod<T>)
        {
            argument.toStream(stream);
        }
        else if constexpr (HasToWideStreamFunction<T>)
        {
            stream << argument;
        }
        else if constexpr (HasToWideStringMethod<T>)
        {
            stream << argument.toWideString();
        }
        else if constexpr (HasToStringMethod<T>)
        {
            const auto string = argument.toString();
            stream << string.c_str();
        }
        else
        {
            static_assert(!"Cannot format type");
        }
    };

    (writer(arguments), ...);

    return stream.str();
}

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

PRALINE_EXPORT std::optional<std::string> getEnvironmentVariable(const std::string& variable);

}
