#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ui/theme/theme_listener.h"
#include "ui/theme/theme_manager.h"

// Will be worked on

namespace dawn::ui::control
{

    class DawnButton : public juce::Button, public theme::ThemeListener
    {
      public:
        explicit DawnButton(const juce::String& name, const theme::ButtonStyleID styleID)
            : Button(name), styleID_(styleID)
        {
            theme::ThemeManager::addListener(this);
        }

        ~DawnButton() override { theme::ThemeManager::removeListener(this); }

        void setStyle(const theme::ButtonStyleID styleID) { styleID_ = styleID; }

        void onThemeChange() override;

      protected:
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;

      private:
        theme::ButtonStyleID styleID_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DawnButton)
    };
} // namespace dawn::ui::control