#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "resources.h"

/*
 * Type traits for resource IDs.
 *
 * Associates each resource ID type with the value type managed by the resource system.
 *
 * Add a ResourceTrait template specialization when introducing a new resource ID type.
 */

namespace dawn::core::detail
{
    template <typename> struct ResourceTrait;

    template <> struct ResourceTrait<resources::ImageID>
    {
        using ValueType = juce::Image;
    };

    template <typename T>
    concept ResourceID = requires { typename ResourceTrait<T>::ValueType; };
} // namespace dawn::core::detail
