#pragma once

/// @file colorizer.hpp
/// @brief Colorizer with static color registry for level-based ANSI colorization.

#include <map>
#include <mutex>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Manages a global color registry and colorizes strings by level.
///
/// Uses polycpp::util::styleText() for applying ANSI escape codes and
/// polycpp::util::stripVTControlCharacters() for stripping them.
///
/// The color registry is thread-safe (protected by a mutex).
/// Colors are registered globally via addColors() and shared across
/// all Colorizer instances.
///
/// @see https://github.com/winstonjs/logform/blob/master/colorize.js
class Colorizer {
public:
    /// @brief Register colors for levels (merges with existing registry).
    ///
    /// Color values are space-separated style names compatible with
    /// polycpp::util::styleText (e.g., "bold red", "cyan", "underline yellow").
    ///
    /// @param colors Map of level name -> space-separated color/style names.
    static void addColors(const std::map<std::string, std::string>& colors);

    /// @brief Get all registered colors.
    /// @return Copy of the current color registry.
    static std::map<std::string, std::string> allColors();

    /// @brief Colorize a string using the color(s) registered for a level.
    ///
    /// If no color is registered for the level, returns the string unchanged.
    /// Multiple styles (space-separated) are applied left-to-right (nested).
    ///
    /// @param level The level whose color to use.
    /// @param text The text to colorize.
    /// @return The colorized string with ANSI escape codes.
    static std::string colorize(const std::string& level, const std::string& text);

    /// @brief Strip all ANSI escape codes from a string.
    ///
    /// Uses polycpp::util::stripVTControlCharacters().
    ///
    /// @param str The string to strip.
    /// @return The string with all ANSI codes removed.
    static std::string strip(const std::string& str);

private:
    /// Global color registry: level name -> space-separated style names.
    static std::map<std::string, std::string> allColors_;

    /// Mutex protecting the global color registry.
    static std::mutex mutex_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/colorizer.hpp>
