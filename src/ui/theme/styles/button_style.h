#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "resources.h"

namespace dawn::ui::theme
{
    struct ButtonStateStyle
    {
        juce::Colour background;
        juce::Colour foreground;
        juce::Colour border;
    };

    struct ButtonStyle
    {
        ButtonStateStyle normal;
        ButtonStateStyle hover;
        ButtonStateStyle pressed;
        ButtonStateStyle disabled;

        float fontSize = 14.0f;
        float cornerRadius{};
        float borderThickness{};
    };
} // namespace dawn::ui::theme