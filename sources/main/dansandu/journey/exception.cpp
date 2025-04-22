#include "dansandu/journey/exception.hpp"

#include <iostream>
#include <stdexcept>

namespace dansandu::journey::exception
{

int wrapInTryCatch(const std::function<int(const int, const char* const* const)>& callback, const int argumentCount,
                   const char* const* const arguments)
{
    try
    {
        return callback(argumentCount, arguments);
    }
    catch (const Exception& exception)
    {
        std::wcerr << "Exception was thrown with message: " << exception.message() << std::endl;
        return 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Exception was thrown with message: " << exception.what() << std::endl;
        return 1;
    }
}

}
