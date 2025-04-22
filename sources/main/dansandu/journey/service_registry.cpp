#include "dansandu/journey/service_registry.hpp"

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>

namespace dansandu::journey::service_registry
{

static std::map<std::string, ServiceRegistry::ServiceInvokerType> services;

static std::mutex servicesMutex;

int ServiceRegistry::registerServiceInvoker(const std::string& serviceIdentifier,
                                            const ServiceInvokerType serviceInvoker)
{
    const auto lock = std::lock_guard<std::mutex>(servicesMutex);
    const auto position = services.find(serviceIdentifier);
    if (position != services.end())
    {
        throw std::logic_error{"identifier '" + serviceIdentifier + "' is used by another service"};
    }
    services.insert({serviceIdentifier, serviceInvoker});
    return 0;
}

ServiceRegistry::ServiceInvokerType ServiceRegistry::getServiceInvoker(const std::string& identifier)
{
    const auto lock = std::lock_guard<std::mutex>(servicesMutex);
    const auto position = services.find(identifier);
    if (position == services.end())
    {
        throw std::logic_error{"no service was registered with identifier '" + identifier + "'"};
    }
    return position->second;
}

}
