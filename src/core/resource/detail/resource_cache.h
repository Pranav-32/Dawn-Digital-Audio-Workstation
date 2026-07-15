#pragma once

#include <array>
#include <utility>

#include "resource_traits.h"

/*
 * Provides the cache storage for embedded resources.
 *
 * Each ResourceID specialization owns a statically allocated cache whose size
 * is determined at compile time from the generated ResourceID::Count value for embedded resources.
 *
 * The cache stores the runtime JUCE objects (e.g. juce::Image) created from
 * the embedded resource data during ResourceManager::init().
 *
 * This is an internal implementation detail. Resources should be accessed
 * through:
 *
 *     dawn::core::ResourceManager::getResource(ResourceID)
 *
 * TODO:
 * - Dynamic resource caching.
 */

namespace dawn::core::detail
{
    template <ResourceID T> struct ResourceCache
    {
        using ValueType = ResourceTrait<T>::ValueType;

        inline static std::array<ValueType, std::to_underlying(T::Count)> cache;

        static ValueType& at(const T id)
        {
            jassert(std::to_underlying(id) < cache.size());

            return cache[std::to_underlying(id)];
        }
    };
} // namespace dawn::core::detail
