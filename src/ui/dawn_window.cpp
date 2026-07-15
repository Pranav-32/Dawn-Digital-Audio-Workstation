#include "dawn_window.h"

#include "core/logger.h"
#include "theme/theme_manager.h"

namespace dawn::ui
{
    DawnGUIWindow::DawnGUIWindow(const juce::String& name)
        : DocumentWindow(name, juce::Colour(juce::Colours::aliceblue), allButtons)
    {
        core::Logger::Debug("Initializing GUI Window");

        setOpaque(true);
        setUsingNativeTitleBar(false);

        setTitleBarHeight(0);

        setResizable(true, false);
        setFullScreen(false);

        setDraggable(false);

        Component::setVisible(true);

        core::Logger::Debug("GUI Window initialized and running");
    }

    void DawnGUIWindow::closeButtonPressed()
    {
        core::Logger::Debug("Exiting GUI Window");
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

    void DawnGUIWindow::resized() {}
} // namespace dawn::ui
