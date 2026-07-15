#pragma once

/*
 * Interface for objects that receive theme change notifications.
 *
 * Classes interested in theme updates should inherit from ThemeListener and
 * register themselves with ThemeManager::addListener(ThemeListener*).
 *
 * Registered listeners are notified through the onThemeChange() override whenever the active
 * theme changes.
 *
 * Listeners should unregister themselves before destruction, or from their destructor, to avoid
 * receiving further notifications.
 */

namespace dawn::ui::theme
{
    class ThemeListener
    {
      public:
        ThemeListener() = default;

        ThemeListener(const ThemeListener&) = delete;
        ThemeListener& operator=(const ThemeListener&) = delete;

        virtual ~ThemeListener() = default;

        virtual void onThemeChange() = 0;
    };
} // namespace dawn::ui::theme