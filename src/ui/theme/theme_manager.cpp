#include "theme_manager.h"

#include "core/logger.h"

/*
 * ThemeManager implementation.
 *
 * Manages listener registration and dispatches theme change notifications to all registered
 * ThemeListeners.
 */

namespace dawn::ui::theme
{
    void ThemeManager::addListener(ThemeListener* listener) noexcept
    {
        jassert(listener != nullptr);
        listeners_.add(listener);
    }

    void ThemeManager::removeListener(ThemeListener* listener) noexcept
    {
        jassert(listener != nullptr);
        listeners_.remove(listener);
    }

    void ThemeManager::notifyListeners() noexcept
    {
        core::Logger::Debug("Notifying theme listeners");
        listeners_.call([](ThemeListener& l) { l.onThemeChange(); });
    }
} // namespace dawn::ui::theme
