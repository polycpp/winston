#pragma once

/// @file console_transport.hpp
/// @brief ConsoleTransport declaration — writes log entries to stdout/stderr.

#include <polycpp/winston/transport.hpp>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

namespace polycpp {
namespace winston {

/// @brief Options for ConsoleTransport construction.
///
/// Extends the base TransportOptions fields with console-specific settings
/// for stderr routing and end-of-line control.
///
/// @see https://github.com/winstonjs/winston/blob/master/lib/winston/transports/console.js
struct ConsoleTransportOptions {
    /// Per-transport level threshold. Empty string means inherit from logger.
    std::string level;

    /// Per-transport format (applied after logger format, before log()).
    std::shared_ptr<Format> format;

    /// Suppress this transport entirely.
    bool silent = false;

    /// Whether this transport handles uncaught exceptions.
    bool handleExceptions = false;

    /// Whether this transport handles unhandled rejections.
    bool handleRejections = false;

    /// @brief Levels routed to stderr.
    ///
    /// If a log entry's originalLevel is in this set, the output is written
    /// to stderr instead of stdout. Default: empty (all output to stdout).
    std::vector<std::string> stderrLevels;

    /// @brief Levels routed to stderr via console.warn equivalent.
    ///
    /// If a log entry's originalLevel is in this set (and not in stderrLevels),
    /// the output is written to stderr. Default: empty.
    std::vector<std::string> consoleWarnLevels;

    /// @brief End-of-line string appended to each log line.
    ///
    /// Default: polycpp::os::EOL (platform-appropriate line ending).
    /// Set to an empty string to suppress, or to a custom value.
    std::string eol;
};

/// @brief Transport that writes log entries to console streams (stdout/stderr).
///
/// The most commonly used winston transport. By default, all output goes to
/// stdout. Specific levels can be routed to stderr via the stderrLevels and
/// consoleWarnLevels options.
///
/// @par Example
/// ```cpp
/// ConsoleTransportOptions opts;
/// opts.stderrLevels = {"error"};
/// auto transport = std::make_shared<ConsoleTransport>(opts);
/// ```
///
/// @see https://github.com/winstonjs/winston/blob/master/docs/transports.md#console-transport
class ConsoleTransport : public Transport {
public:
    /// @brief Construct with custom options.
    ///
    /// All output goes to std::cout and std::cerr. Level routing is
    /// determined by the stderrLevels and consoleWarnLevels options.
    ///
    /// @param opts Console transport configuration options.
    explicit ConsoleTransport(const ConsoleTransportOptions& opts = {});

    /// @brief Construct with injectable output streams (for testing).
    ///
    /// @param opts Console transport configuration options.
    /// @param out Stream for stdout-bound messages.
    /// @param err Stream for stderr-bound messages.
    ConsoleTransport(const ConsoleTransportOptions& opts,
                     std::ostream& out, std::ostream& err);

    /// @brief Write a log entry to the appropriate output stream.
    ///
    /// Selects stdout or stderr based on the log entry's originalLevel
    /// and the configured stderrLevels/consoleWarnLevels. Appends the
    /// configured eol string after each message.
    ///
    /// @param info The fully formatted log entry.
    void log(const LogInfo& info) override;

private:
    std::ostream& stdout_;
    std::ostream& stderr_;
    std::unordered_set<std::string> stderrLevels_;
    std::unordered_set<std::string> consoleWarnLevels_;
    std::string eol_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/console_transport.hpp>
