#pragma once

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace dansandu::journey::utility
{

// Used to pass strings as template parameters.
template<std::size_t N>
struct FixedString
{
    constexpr FixedString(const char (&string)[N]) noexcept
    {
        std::copy_n(string, N, data);
    }

    char data[N];
};

template<typename T>
concept HasToStreamMethod =
    requires { static_cast<void (std::decay_t<T>::*)(std::ostream&) const>(&std::decay_t<T>::toStream); };

template<typename T>
concept HasToStringMethod =
    requires { static_cast<std::string (std::decay_t<T>::*)() const>(&std::decay_t<T>::toString); };

template<typename T>
concept HasToStreamFunction = requires(std::ostream stream, const T value) { stream << value; };

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

PRALINE_EXPORT void writeToStandardOutput(const std::string_view string, const bool flush = false);

PRALINE_EXPORT void writeToStandardError(const std::string_view string, const bool flush = false);

PRALINE_EXPORT std::string getFileName(const std::string_view path);

PRALINE_EXPORT bool isSubpath(const std::string_view path, const std::string_view rootPath);

PRALINE_EXPORT bool tryGetRelativePath(const std::string_view path, const std::string_view rootPath,
                                       std::string& relativePath);

PRALINE_EXPORT std::string replaceBackSlashes(const std::string_view path);

PRALINE_EXPORT std::optional<std::string> getEnvironmentVariable(const std::string& variable);

PRALINE_EXPORT bool tryDemangle(const std::string& symbol, const bool includeNamespace, std::string& output);

template<typename Type>
std::string getTypeName(const bool includeNamespace)
{
    const auto mangled = typeid(Type).name();
    auto demangled = std::string{mangled};
    if (::dansandu::journey::utility::tryDemangle(mangled, includeNamespace, demangled))
    {
        return demangled;
    }
    return "<Unknown>";
}

template<typename Type>
std::string getTypeName(const bool includeNamespace, const Type& type)
{
    const auto mangled = typeid(type).name();
    auto demangled = std::string{mangled};
    if (::dansandu::journey::utility::tryDemangle(mangled, includeNamespace, demangled))
    {
        return demangled;
    }
    return "<Unknown>";
}

}
