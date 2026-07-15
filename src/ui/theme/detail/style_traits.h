#pragma once

#include "../ids/all_ids.h"
#include "../styles/all_style.h"

/*
 * Type traits for style IDs.
 *
 * Maps each style ID type to its associated style value type and defines the StyleID concept
 * used to constrain theme-related APIs.
 *
 * Extend StyleTrait by adding a template specialization whenever a new style ID type is introduced.
 */

namespace dawn::ui::theme::detail
{
    template <typename> struct StyleTrait;

    template <> struct StyleTrait<ButtonStyleID>
    {
        using ValueType = ButtonStyle;
    };

    template <typename T>
    concept StyleID = requires { typename StyleTrait<T>::ValueType; };
} // namespace dawn::ui::theme::detail