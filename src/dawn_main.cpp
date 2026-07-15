#include <juce_events/juce_events.h>

#include "core/logger.h"
#include "core/resource/resource_manager.h"

#include "ui/dawn_window.h"
#include "ui/theme/theme_manager.h"
#include "ui/theme/themes/dark.h"

#if JUCE_WINDOWS
#if defined(_DEBUG) || defined(DEBUG)
#include <fcntl.h>
#include <windows.h>
#endif
#endif

class DawnApplication : public juce::JUCEApplication
{
  public:
    DawnApplication() = default;

    const juce::String getApplicationName() override { return "DAWN Audio"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; };
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLineParameters) override
    {
#if JUCE_WINDOWS
#if defined(_DEBUG) || defined(DEBUG)
        if (AllocConsole())
        {
            FILE* dummy;
            freopen_s(&dummy, "CONOUT$", "w", stdout);
            freopen_s(&dummy, "CONOUT$", "w", stderr);
        }
#endif
#endif

        dawn::core::Logger::Debug("Initializing application");

        dawn::core::ResourceManager::init();
        dawn::ui::theme::ThemeManager::setTheme<dawn::ui::theme::DarkTheme>();

        dawn::core::Logger::Debug("Creating GUI Window");
        guiWindow_ = std::make_unique<dawn::ui::DawnGUIWindow>(getApplicationName());

        if (auto* peer = guiWindow_->getPeer())
        {
            peer->setIcon(
                dawn::core::ResourceManager::getResource(dawn::resources::ImageID::dawn_logo));
        }
        guiWindow_->setIcon(
            dawn::core::ResourceManager::getResource(dawn::resources::ImageID::dawn_logo));
    }

    void shutdown() override
    {
        dawn::core::Logger::Debug("Shutting down application");
        guiWindow_ = nullptr;

#if JUCE_WINDOWS
#if defined(_DEBUG) || defined(DEBUG)
        FreeConsole();
#endif
#endif
    }

    void systemRequestedQuit() override { quit(); }

  private:
    std::unique_ptr<dawn::ui::DawnGUIWindow> guiWindow_;
};

START_JUCE_APPLICATION(DawnApplication)