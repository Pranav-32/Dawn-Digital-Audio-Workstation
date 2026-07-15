#include "title_bar.h"

#include "ui/theme/theme_manager.h"

namespace dawn::ui::component
{
    TitleBar::TitleBar() { setOpaque(true); }

    void TitleBar::paint(juce::Graphics& g) { g.fillAll(juce::Colours::black); }

    void TitleBar::resized() {}

    void TitleBar::mouseDown(const juce::MouseEvent& event)
    {
        if (auto* topWindow = findParentComponentOfClass<juce::DocumentWindow>())
        {
            dragger_.startDraggingComponent(topWindow, event);
        }
    }

    void TitleBar::mouseDrag(const juce::MouseEvent& event)
    {
        if (auto* topWindow = findParentComponentOfClass<juce::DocumentWindow>())
        {
            dragger_.dragComponent(topWindow, event, &constrainer_);
        }
    }
} // namespace dawn::ui::component
