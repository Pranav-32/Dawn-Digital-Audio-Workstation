#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Will be worked on

namespace dawn::ui::component
{
    class TitleBar : public juce::Component
    {
      public:
        TitleBar();
        ~TitleBar() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override;

      private:
        juce::ComponentDragger dragger_;
        juce::ComponentBoundsConstrainer constrainer_;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleBar)
    };
} // namespace dawn::ui::component
