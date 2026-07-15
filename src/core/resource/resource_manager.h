#pragma once

#include <mutex>

#include "detail/resource_loader.h"

/*
 * Public interface for DAWN's embedded resource system.
 *
 * Provides type-safe, thread-safe access to cached runtime resources created
 * from the embedded asset manifest.
 *
 * Resources are retrieved using their generated ResourceID, providing
 * compile-time type safety and constant-time lookup.
 *
 * Embedded resources are initialized exactly once through init() and remain
 * immutable for the lifetime of the application. The caller must ensure
 * init() has completed before calling getResource().
 *
 * Example:
 *
 *     auto& logo =
 *         ResourceManager::getResource(resources::ImageID::dawn_logo);
 *
 * Dynamic resource loading will be introduced separately without affecting
 * the public API.
 *
 * TODO:
 * - Add support for dynamic resource management.
 */

namespace dawn::core
{
    class ResourceManager
    {
      public:
        ResourceManager() = delete;

        static void init() noexcept;

        template <detail::ResourceID T>
        [[nodiscard]]
        static const auto& getResource(T id) noexcept
        {
            return detail::ResourceCache<T>::at(id);
        }

      private:
        inline static std::once_flag initFlag_;
    };
} // namespace dawn::core