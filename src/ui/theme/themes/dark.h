#pragma once

#include "ui/theme/detail/theme_bundle.h"
#include "ui/theme/ids/all_ids.h"

namespace dawn::ui::theme
{
    class DarkTheme
    {
      public:
        static void build(detail::ThemeBundle& bundle) noexcept
        {
            bundle.getStyle(ButtonStyleID::win_controls_button) = {
                .normal = {.background = juce::Colour(0xFFC2C3C2),
                           .foreground = juce::Colours::aliceblue,
                           .border = juce::Colour(0)},
                .cornerRadius = 5.0f};
        }
    };
} // namespace dawn::ui::theme