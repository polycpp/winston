#pragma once

/// @file level_config.hpp
/// @brief Level configuration presets (npm, syslog, cli) and custom level definitions.

#include <map>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Defines a level-to-priority mapping and associated colors.
///
/// Lower numeric priority = higher severity. A log message passes if
/// `levels[configuredLevel] >= levels[messageLevel]`.
///
/// @see https://github.com/winstonjs/winston#logging-levels
struct LevelConfig {
    /// Level name -> numeric priority. Lower number = more severe.
    std::map<std::string, int> levels;

    /// Level name -> color name(s). Values are space-separated style names
    /// compatible with polycpp::util::styleText (e.g., "bold red", "cyan").
    std::map<std::string, std::string> colors;

    /// @brief npm logging levels (default).
    ///
    /// error(0), warn(1), info(2), http(3), verbose(4), debug(5), silly(6)
    ///
    /// @return LevelConfig with npm levels and colors.
    static LevelConfig npm();

    /// @brief Syslog levels (RFC 5424).
    ///
    /// emerg(0), alert(1), crit(2), error(3), warning(4), notice(5), info(6), debug(7)
    ///
    /// @return LevelConfig with syslog levels and colors.
    static LevelConfig syslog();

    /// @brief CLI levels (extended range for interactive apps).
    ///
    /// error(0), warn(1), help(2), data(3), info(4), debug(5),
    /// prompt(6), verbose(7), input(8), silly(9)
    ///
    /// @return LevelConfig with cli levels and colors.
    static LevelConfig cli();

    /// @brief Check if a message at `messageLevel` should pass given the
    /// configured `thresholdLevel`.
    ///
    /// Returns true if `levels[thresholdLevel] >= levels[messageLevel]`,
    /// meaning the message is severe enough to be logged.
    ///
    /// @param thresholdLevel The configured level threshold.
    /// @param messageLevel The level of the incoming message.
    /// @return True if the message should be logged.
    bool isLevelEnabled(const std::string& thresholdLevel,
                        const std::string& messageLevel) const;

    /// @brief Get the maximum numeric level value (lowest severity).
    /// @return The highest numeric priority value, or -1 if levels is empty.
    int maxLevel() const;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/level_config.hpp>
