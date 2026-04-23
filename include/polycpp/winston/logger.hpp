#pragma once

/// @file logger.hpp
/// @brief Logger class -- the central orchestrator of the winston logging library.

#include <polycpp/winston/log_info.hpp>
#include <polycpp/winston/format.hpp>
#include <polycpp/winston/transport.hpp>
#include <polycpp/winston/level_config.hpp>
#include <polycpp/winston/events.hpp>
#include <polycpp/events.hpp>
#include <polycpp/core/json.hpp>
#include <polycpp/core/error.hpp>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace polycpp {
namespace winston {

// Forward declarations
class Profiler;
class ChildLogger;

/// @brief Configuration options for constructing a Logger.
///
/// Maps to winston's createLogger() options object.
///
/// @see https://github.com/winstonjs/winston#creating-your-own-logger
struct LoggerOptions {
    /// The maximum level of messages to log. Messages with a higher numeric
    /// priority (lower severity) are dropped.
    std::string level = "info";

    /// Level name-to-priority mapping and associated colors.
    LevelConfig levels = LevelConfig::npm();

    /// Logger-level format applied before transport dispatch.
    /// nullptr means no format (pass through unchanged).
    std::shared_ptr<Format> format;

    /// Transports to dispatch log entries to.
    std::vector<std::shared_ptr<Transport>> transports;

    /// Suppress all output when true.
    bool silent = false;

    /// Default metadata merged into every log entry.
    JsonObject defaultMeta;

    /// Exit behavior on uncaught errors. Reserved for future use with ExceptionHandler.
    /// Currently stored but not enforced -- transport errors are emitted as "error" events.
    /// - true: exit on error (default)
    /// - false: do not exit
    /// - function: custom predicate returning true to exit
    std::variant<bool, std::function<bool(const polycpp::Error&)>> exitOnError = true;

    /// Whether to handle uncaught exceptions.
    bool handleExceptions = false;

    /// Whether to handle unhandled promise rejections.
    bool handleRejections = false;
};

/// @brief Central logging class that orchestrates format pipeline, transport
/// dispatch, level filtering, profiling, and event emission.
///
/// Extends polycpp::EventEmitter for events:
/// - "error" -- forwarded from transports
/// - "warn"  -- forwarded from transports
/// - "close" -- emitted when close() is called
///
/// Uses direct dispatch to transports (not stream-based).
///
/// @par Example
/// ```cpp
/// auto transport = std::make_shared<ConsoleTransport>();
/// Logger logger(LoggerOptions{
///     .level = "info",
///     .transports = {transport}
/// });
/// logger.info("Server started", {{"port", JsonValue(3000)}});
/// ```
///
/// @see https://github.com/winstonjs/winston#creating-your-own-logger
class Logger : public polycpp::events::EventEmitter {
public:
    /// @brief Construct a Logger with the given options.
    /// @param options Configuration options.
    explicit Logger(LoggerOptions options = {});

    /// @brief Destructor. Cleans up transport error listeners to prevent dangling captures.
    virtual ~Logger();

    // --- Core logging ---

    /// @brief Log a pre-built LogInfo entry.
    ///
    /// Checks silent mode, validates level, applies level filtering,
    /// merges defaultMeta, then delegates to write() for format and dispatch.
    ///
    /// @param info The log entry.
    /// @return Reference to this for chaining.
    Logger& log(LogInfo info);

    /// @brief Log at a specified level with a message.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    /// @return Reference to this for chaining.
    Logger& log(const std::string& level, const std::string& message);

    /// @brief Log at a specified level with a message and metadata.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    /// @param meta Additional metadata for this log entry.
    /// @return Reference to this for chaining.
    Logger& log(const std::string& level, const std::string& message,
                const JsonObject& meta);

    // --- Fixed npm level methods ---

    /// @brief Log at error level (npm severity 0).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& error(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at warn level (npm severity 1).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& warn(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at info level (npm severity 2).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& info(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at http level (npm severity 3).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& http(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at verbose level (npm severity 4).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& verbose(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at debug level (npm severity 5).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& debug(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at silly level (npm severity 6).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& silly(const std::string& message, const JsonObject& meta = {});

    // --- Fixed syslog level methods ---

    /// @brief Log at emerg level (syslog severity 0).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& emerg(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at alert level (syslog severity 1).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& alert(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at crit level (syslog severity 2).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& crit(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at warning level (syslog severity 4).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& warning(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at notice level (syslog severity 5).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    Logger& notice(const std::string& message, const JsonObject& meta = {});

    // --- Transport management ---

    /// @brief Add a transport to this logger.
    ///
    /// Sets the transport's level config and wires up event forwarding
    /// (transport "error" events are re-emitted on the logger).
    ///
    /// @param transport The transport to add.
    /// @return Reference to this for chaining.
    Logger& add(std::shared_ptr<Transport> transport);

    /// @brief Remove a transport from this logger.
    ///
    /// @param transport The transport to remove (matched by pointer identity).
    /// @return Reference to this for chaining.
    Logger& remove(std::shared_ptr<Transport> transport);

    /// @brief Remove all transports from this logger.
    /// @return Reference to this for chaining.
    Logger& clear();

    /// @brief Close the logger: remove all transports and emit "close".
    void close();

    // --- Configuration ---

    /// @brief Reconfigure the logger with new options.
    ///
    /// Clears all existing transports, then applies the new configuration.
    ///
    /// @param options New configuration options.
    void configure(const LoggerOptions& options);

    // --- Level checking ---

    /// @brief Check if a message at the given level would be logged.
    ///
    /// @param level The level to check.
    /// @return True if the level is enabled.
    bool isLevelEnabled(const std::string& level) const;

    /// @brief Check if error level is enabled.
    /// @return True if error messages would be logged.
    bool isErrorEnabled() const;

    /// @brief Check if warn level is enabled.
    /// @return True if warn messages would be logged.
    bool isWarnEnabled() const;

    /// @brief Check if info level is enabled.
    /// @return True if info messages would be logged.
    bool isInfoEnabled() const;

    /// @brief Check if http level is enabled.
    /// @return True if http messages would be logged.
    bool isHttpEnabled() const;

    /// @brief Check if verbose level is enabled.
    /// @return True if verbose messages would be logged.
    bool isVerboseEnabled() const;

    /// @brief Check if debug level is enabled.
    /// @return True if debug messages would be logged.
    bool isDebugEnabled() const;

    /// @brief Check if silly level is enabled.
    /// @return True if silly messages would be logged.
    bool isSillyEnabled() const;

    // --- Profiling ---

    /// @brief Toggle-based profiling.
    ///
    /// First call with an id starts a timer. Second call with the same id
    /// stops the timer and logs the elapsed time with durationMs in metadata.
    ///
    /// @param id The profiler identifier.
    void profile(const std::string& id);

    /// @brief Toggle-based profiling with metadata.
    ///
    /// Same as profile(id), but merges extra metadata when logging the result.
    ///
    /// @param id The profiler identifier.
    /// @param meta Additional metadata to include when logging the duration.
    void profile(const std::string& id, const JsonObject& meta);

    /// @brief Create a Profiler that starts timing immediately.
    ///
    /// Call done() on the returned Profiler to log the elapsed time.
    ///
    /// @return A Profiler bound to this logger.
    Profiler startTimer();

    // --- Child loggers ---

    /// @brief Create a child logger with additional default metadata.
    ///
    /// @param defaultMeta Metadata to merge into every log entry from the child.
    /// @return A ChildLogger bound to this logger.
    ChildLogger child(const JsonObject& defaultMeta);

    // --- Accessors ---

    /// @brief Get the current level threshold.
    /// @return The level name.
    const std::string& level() const;

    /// @brief Set the level threshold.
    /// @param lvl The new level name.
    void setLevel(const std::string& lvl);

    /// @brief Get the level configuration.
    /// @return Reference to the LevelConfig.
    const LevelConfig& levels() const;

    /// @brief Get the list of transports.
    /// @return Reference to the transport vector.
    const std::vector<std::shared_ptr<Transport>>& transports() const;

    /// @brief Get the silent flag.
    /// @return True if the logger is silent.
    bool silent() const;

    /// @brief Set the silent flag.
    /// @param s True to suppress all output.
    void setSilent(bool s);

    /// @brief Get the default metadata.
    /// @return Reference to the default metadata.
    const JsonObject& defaultMeta() const;

    /// @brief Set the default metadata.
    /// @param meta New default metadata.
    void setDefaultMeta(const JsonObject& meta);

    /// @brief Get the logger-level format.
    /// @return Shared pointer to the format (may be nullptr).
    const std::shared_ptr<Format>& format() const;

protected:
    /// @brief Dispatch a log entry through the format pipeline and transports.
    ///
    /// Called by log() after level check and metadata merge. ChildLogger
    /// can intercept before this point to add its own metadata.
    ///
    /// @param info The log entry to dispatch.
    virtual void write(LogInfo info);

private:
    std::string level_;
    LevelConfig levels_;
    std::shared_ptr<Format> format_;
    std::vector<std::shared_ptr<Transport>> transports_;
    JsonObject defaultMeta_;
    bool silent_;
    std::variant<bool, std::function<bool(const polycpp::Error&)>> exitOnError_;
    std::map<std::string, double> profilers_;  ///< id -> Date::now() start time

    /// @brief Listener IDs for event forwarding from transports.
    /// Maps transport pointer to the listener ID on that transport.
    std::map<Transport*, polycpp::ListenerId> errorListenerIds_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/logger.hpp>
