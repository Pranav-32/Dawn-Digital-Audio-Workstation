#include "resource_manager.h"

#include <mutex>
#include <variant>

#include "core/logger.h"

/*
 * Initializes the embedded resource cache.
 *
 * Iterates over the generated resource manifest and dispatches each resource
 * descriptor to its corresponding loader. Every loader converts the embedded
 * resource data into its runtime representation and stores it in the
 * appropriate cache slot.
 *
 * This implementation relies on std::variant visitation, allowing new
 * resource types to be supported by simply providing a corresponding
 * loadAndCacheResource() overload.
 */

namespace dawn::core
{
    void ResourceManager::init() noexcept
    {
        std::call_once(initFlag_,
                       []
                       {
                           Logger::Debug("Loading and caching embedded resources...");
                           for (const auto& resource : resources::asset_manifest)
                           {
                               std::visit([](const auto& concreteResource)
                                          { detail::loadAndCacheResource(concreteResource); },
                                          resource);
                           }
                           Logger::Debug("Embedded resources initialized");
                       });
    }
} // namespace dawn::core