#pragma once

#include <array>
#include <utility>

#include "style_traits.h"
#include "ui/theme/ids/all_ids.h"

/*
 * Storage for all theme styles managed by ThemeManager.
 *
 * ThemeBundle provides strongly typed access to style storage through style IDs, allowing themes
 * to build and update styles without exposing the underlying storage implementation.
 *
 * To introduce a new style type:
 * - Create the associated style ID in ids/.
 * - Create the associated style struct in styles/.
 * - Add a StyleTrait specialization in 'style_traits.h'.
 * - Add the style storage and getStyle() overload in ThemeBundle.
 *
 * Consider including new IDs and styles in all_ids.h and all_styles.h for convenience.
 */

namespace dawn::ui::theme::detail
{
    template <StyleID T> inline constexpr std::size_t StyleCount = std::to_underlying(T::Count);

    template <StyleID T> struct StyleStorage
    {
        using ValueType = StyleTrait<T>::ValueType;

        std::array<ValueType, StyleCount<T>> styles;

        ValueType& operator[](const T id) noexcept
        {
            jassert(std::to_underlying(id) < styles.size());
            return styles[std::to_underlying(id)];
        }

        const ValueType& operator[](const T id) const noexcept
        {
            jassert(std::to_underlying(id) < styles.size());
            return styles[std::to_underlying(id)];
        }
    };

    struct ThemeBundle
    {
        StyleStorage<ButtonStyleID> buttonStyles;
        [[nodiscard]]
        auto& getStyle(const ButtonStyleID id) noexcept
        {
            return buttonStyles[id];
        }
        [[nodiscard]]
        const auto& getStyle(const ButtonStyleID id) const noexcept
        {
            return buttonStyles[id];
        }
    };

    template <typename T>
    concept ThemeType = requires(ThemeBundle& bundle) {
        { T::build(bundle) } -> std::same_as<void>;
    };
} // namespace dawn::ui::theme::detail