#pragma once

/// @file profiler.hpp
/// @brief Profiler class for timing operations and logging duration.

#include <polycpp/winston/log_info.hpp>
#include <string>

namespace polycpp {
namespace winston {

// Forward declaration
class Logger;

/// @brief Timer-based profiler that logs elapsed time on completion.
///
/// Created by Logger::startTimer(). On calling done(), computes the
/// elapsed time since construction and logs a message with durationMs
/// in the metadata.
///
/// @par Example
/// ```cpp
/// auto profiler = logger.startTimer();
/// // ... perform work ...
/// profiler.done("Operation completed");
/// // logs: { level: "info", message: "Operation completed", durationMs: 42 }
/// ```
///
/// @see https://github.com/winstonjs/winston#profiling
class Profiler {
public:
    /// @brief Construct a profiler bound to a logger.
    ///
    /// Records the current time via polycpp::Date::now().
    ///
    /// @param logger Reference to the Logger to log the result to.
    explicit Profiler(Logger& logger);

    /// @brief Complete the profiler and log the elapsed time.
    ///
    /// Creates a LogInfo with level "info", the given message, and
    /// durationMs in metadata. Delegates to the bound logger's log().
    ///
    /// @param message The message to log (default: empty string).
    void done(const std::string& message = "");

    /// @brief Complete the profiler and log with a custom LogInfo.
    ///
    /// If info.level is empty, defaults to "info". Adds durationMs
    /// to info.metadata and delegates to the bound logger's log().
    ///
    /// @param info The LogInfo to augment with durationMs and log.
    void done(LogInfo info);

private:
    Logger& logger_;
    double start_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/profiler.hpp>
