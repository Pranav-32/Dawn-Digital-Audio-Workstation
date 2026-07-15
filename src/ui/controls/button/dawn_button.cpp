#include "dawn_button.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include "ui/theme/theme_manager.h"

namespace dawn::ui::control
{
    void DawnButton::onThemeChange() { repaint(); }

    void DawnButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                                 bool shouldDrawButtonAsDown)
    {
        const auto& style = theme::ThemeManager::getStyle(styleID_);
    }
} // namespace dawn::ui::control
