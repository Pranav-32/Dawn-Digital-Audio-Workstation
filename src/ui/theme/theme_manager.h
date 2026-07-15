#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "common/helpers.h"
#include "core/logger.h"
#include "detail/theme_bundle.h"
#include "theme_listener.h"

/*
 * Public interface for DAWN's runtime theme system.
 *
 * Owns the active ThemeBundle, provides type-safe access to styles, and notifies registered
 * listeners whenever the active theme changes.
 *
 * Static themes are applied by calling setTheme<T>(), where T is a theme class exposing:
 *
 *     static void build(detail::ThemeBundle&) noexpect;
 *
 * The build function is responsible for configuring all required styles inside the provided
 * ThemeBundle.
 *
 * To introduce a new theme:
 * - Create a new theme class.
 * - Implement a static build(detail::ThemeBundle&) function.
 * - Populate the ThemeBundle with all required styles.
 */

namespace dawn::ui::theme
{
    class ThemeManager
    {
      public:
        /*
         * Applies the specified theme.
         *
         * Calling setTheme() before constructing the UI is recommended so that components are
         * initialized with the correct styles on first paint call.
         *
         * When UI components are already alive, this function must be called from JUCE's message
         * thread, as registered ThemeListeners may call juce::Component APIs.
         */
        template <detail::ThemeType T> static void setTheme() noexcept
        {
            const auto token = common::helpers::typeToken<T>();

            if (currentThemeToken_ == token)
                return;

            currentThemeToken_ = token;
            T::build(currentThemeBundle_);
            core::Logger::Debug("Theme updated");
            notifyListeners();
        }

        template <detail::StyleID T>
        [[nodiscard]]
        static const auto& getStyle(T id) noexcept
        {
            return currentThemeBundle_.getStyle(id);
        }

        static void addListener(ThemeListener* listener) noexcept;
        static void removeListener(ThemeListener* listener) noexcept;

      private:
        inline static const void* currentThemeToken_ = nullptr;
        inline static detail::ThemeBundle currentThemeBundle_;
        inline static juce::ListenerList<ThemeListener> listeners_;

        static void notifyListeners() noexcept;
    };
} // namespace dawn::ui::theme