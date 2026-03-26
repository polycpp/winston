#pragma once

/// @file log_info.hpp
/// @brief LogInfo struct -- the central data object flowing through the format pipeline.

#include <string>
#include <vector>

namespace polycpp {
namespace winston {

/// @brief Represents a single log entry flowing through the format pipeline.
///
/// Maps to winston's `info` object. Named fields replace the Symbol-keyed
/// properties ([LEVEL], [MESSAGE], [SPLAT]) from the JavaScript version.
///
/// @see https://github.com/winstonjs/winston#streams-objectmode-and-info-objects
struct LogInfo {
    /// Mutable level name (may be colorized by the colorize format).
    std::string level;

    /// Immutable original level name (used for filtering after colorization).
    /// Maps to JS `info[Symbol.for('level')]`.
    std::string originalLevel;

    /// The log message (mutable during format pipeline).
    std::string message;

    /// Final formatted output string (set by finalizing formats like json/simple/printf).
    /// Maps to JS `info[Symbol.for('message')]`.
    std::string formattedMessage;
};

} // namespace winston
} // namespace polycpp
