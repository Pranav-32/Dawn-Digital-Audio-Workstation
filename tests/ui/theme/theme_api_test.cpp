#include <catch2/catch_test_macros.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include "ui/theme/theme_manager.h"
#include "ui/theme/themes/dark.h"

using namespace dawn::ui::theme;

namespace
{
    class CustomTheme
    {
      public:
        static void build(detail::ThemeBundle& bundle) noexcept
        {
            bundle.getStyle(ButtonStyleID::win_controls_button) = {
                .normal = {.background = juce::Colour(0xFF123456)},
            };
        }
    };

    class SpyThemeListener : public ThemeListener
    {
      public:
        SpyThemeListener() { ThemeManager::addListener(this); }
        ~SpyThemeListener() override { ThemeManager::removeListener(this); }

        void onThemeChange() override { ++callCount; }

        int callCount = 0;
    };
} // namespace

TEST_CASE("Styles are default-initialized before a theme is applied", "[ui][theme]")
{
    auto& style = ThemeManager::getStyle(ButtonStyleID::win_controls_button);

    REQUIRE(style.normal.background == juce::Colour());
}

TEST_CASE("setTheme updates style values", "[ui][theme]")
{
    ThemeManager::setTheme<DarkTheme>();

    const auto& style = ThemeManager::getStyle(ButtonStyleID::win_controls_button);

    REQUIRE_FALSE(style.normal.background == juce::Colour());
}

TEST_CASE("ThemeManager preserves style reference validity", "[ui][theme]")
{
    const auto* before = &ThemeManager::getStyle(ButtonStyleID::win_controls_button);

    ThemeManager::setTheme<CustomTheme>();

    const auto* after = &ThemeManager::getStyle(ButtonStyleID::win_controls_button);

    REQUIRE(before == after);
}

TEST_CASE("setTheme supports arbitrary theme types", "[ui][theme]")
{
    REQUIRE_NOTHROW(ThemeManager::setTheme<CustomTheme>());

    const auto& style = ThemeManager::getStyle(ButtonStyleID::win_controls_button);

    REQUIRE(style.normal.background == juce::Colour(0xFF123456));
}

TEST_CASE("setTheme mutates existing style objects in-place", "[ui][theme]")
{
    ThemeManager::setTheme<DarkTheme>();

    const auto& style = ThemeManager::getStyle(ButtonStyleID::win_controls_button);

    ThemeManager::setTheme<CustomTheme>();

    REQUIRE(style.normal.background == juce::Colour(0xFF123456));
}

TEST_CASE("ThemeListener is not notified when the active theme is unchanged", "[ui][theme]")
{
    SpyThemeListener testListener;

    ThemeManager::setTheme<DarkTheme>();
    ThemeManager::setTheme<DarkTheme>();

    REQUIRE(testListener.callCount == 1);
}

TEST_CASE("ThemeListener is notified when the active theme changes", "[ui][theme]")
{
    SpyThemeListener testListener;

    ThemeManager::setTheme<CustomTheme>();
    ThemeManager::setTheme<DarkTheme>();

    REQUIRE(testListener.callCount == 2);
}

TEST_CASE("setTheme replaces style values on theme change", "[ui][theme]")
{
    ThemeManager::setTheme<DarkTheme>();

    const auto darkColour =
        ThemeManager::getStyle(ButtonStyleID::win_controls_button).normal.background;

    ThemeManager::setTheme<CustomTheme>();

    REQUIRE(ThemeManager::getStyle(ButtonStyleID::win_controls_button).normal.background !=
            darkColour);
}

TEST_CASE("Removed ThemeListeners are not notified", "[ui][theme]")
{
    SpyThemeListener listener;

    ThemeManager::removeListener(&listener);

    ThemeManager::setTheme<DarkTheme>();

    REQUIRE(listener.callCount == 0);
}

TEST_CASE("ThemeManager notifies all listeners on active theme change", "[ui][theme]")
{
    SpyThemeListener a;
    SpyThemeListener b;
    SpyThemeListener c;

    // ThemeManager persists across test cases, so the initial active theme is
    // unspecified. Apply both themes and compare against the initial call count.

    const auto initialA = a.callCount;
    const auto initialB = b.callCount;
    const auto initialC = c.callCount;

    ThemeManager::setTheme<DarkTheme>();
    ThemeManager::setTheme<CustomTheme>();

    REQUIRE(a.callCount == initialA + 1);
    REQUIRE(b.callCount == initialB + 1);
    REQUIRE(c.callCount == initialC + 1);
}