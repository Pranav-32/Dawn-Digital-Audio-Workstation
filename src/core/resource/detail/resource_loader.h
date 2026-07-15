#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <utility>

#include "resource_cache.h"
#include "resources.h"

/*
 * Loads embedded resources into their runtime cache.
 *
 * Each overload converts one generated resource descriptor into its
 * corresponding JUCE runtime object (e.g. juce::Image) and stores it in
 * the cache slot associated with its ResourceID.
 *
 * Cache bounds are validated via assertions in debug builds before the resource is written.
 *
 * This is an internal implementation detail. Resources should be accessed
 * through:
 *
 *     dawn::core::ResourceManager::getResource(ResourceID)
 *
 * TODO:
 * - Add support for dynamic resource loading.
 * - Improve diagnostics for resource decoding failures.
 */

namespace dawn::core::detail
{
    // Returns the cache slot associated with the ResourceID.
    template <ResourceID T> auto& cacheSlot(T id) noexcept
    {
        jassert(std::to_underlying(id) < ResourceCache<T>::cache.size());

        return ResourceCache<T>::at(id);
    }

    inline void loadAndCacheResource(const resources::ImageResource& resource)
    {
        cacheSlot(resource.id) = juce::ImageFileFormat::loadFrom(resource.bytes, resource.size);
    }
} // namespace dawn::core::detail