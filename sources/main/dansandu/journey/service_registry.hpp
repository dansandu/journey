#pragma once

#include "dansandu/journey/macro.hpp"

#include <string>

namespace dansandu::journey::service_registry
{

class PRALINE_EXPORT ServiceRegistry
{
public:
    using ServiceInvokerType = int (*)(const int argumentCount, const char* const* const arguments);

    static int registerServiceInvoker(const std::string& serviceIdentifier, const ServiceInvokerType serviceInvoker);

    static ServiceInvokerType getServiceInvoker(const std::string& serviceIdentifier);
};

#define DANSANDU_JOURNEY_REGISTER_SERVICE(identifier, invoker)                                                         \
    static int DANSANDU_JOURNEY_UNIQUE_NAME(dansandu_journey_service_registrar_) =                                     \
        dansandu::journey::service_registry::ServiceRegistry::registerServiceInvoker(identifier, invoker)

}
