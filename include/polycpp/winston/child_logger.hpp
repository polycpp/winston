#pragma once

/// @file child_logger.hpp
/// @brief ChildLogger class -- lightweight logger that merges extra metadata.

#include <polycpp/winston/log_info.hpp>
#include <polycpp/core/json.hpp>
#include <string>

namespace polycpp {
namespace winston {

// Forward declaration
class Logger;

/// @brief Lightweight logger that adds default metadata before delegating to a parent Logger.
///
/// Created by Logger::child(). Holds a reference to the parent Logger and
/// a set of default metadata. All log calls merge the child's metadata
/// into the LogInfo before forwarding to the parent.
///
/// Metadata precedence (highest to lowest):
/// 1. Per-log-call metadata (in info.metadata)
/// 2. Child's defaultMeta
/// 3. Parent's defaultMeta (merged by parent Logger::log)
///
/// @note ChildLogger does NOT inherit from Logger. It is a separate class
/// that delegates to a parent Logger. The caller is responsible for keeping
/// the parent Logger alive for the lifetime of the ChildLogger.
///
/// @par Example
/// ```cpp
/// auto child = logger.child({{"requestId", JsonValue("abc-123")}});
/// child.info("handling request");
/// // logs: { level: "info", message: "handling request", requestId: "abc-123" }
/// ```
///
/// @see https://github.com/winstonjs/winston#creating-child-loggers
class ChildLogger {
public:
    /// @brief Construct a child logger with a parent and default metadata.
    ///
    /// @param parent Reference to the parent Logger.
    /// @param defaultMeta Default metadata to merge into every log entry.
    ChildLogger(Logger& parent, const JsonObject& defaultMeta);

    /// @brief Log a pre-built LogInfo object.
    ///
    /// Merges child's defaultMeta into info.metadata (per-call metadata
    /// takes precedence), then delegates to parent.log().
    ///
    /// @param info The log entry to write.
    /// @return Reference to this for chaining.
    ChildLogger& log(LogInfo info);

    /// @brief Log at a specified level with a message.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    /// @return Reference to this for chaining.
    ChildLogger& log(const std::string& level, const std::string& message);

    /// @brief Log at a specified level with a message and metadata.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    /// @param meta Additional metadata for this log entry.
    /// @return Reference to this for chaining.
    ChildLogger& log(const std::string& level, const std::string& message,
                     const JsonObject& meta);

    /// @brief Log at error level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& error(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at warn level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& warn(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at info level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& info(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at http level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& http(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at verbose level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& verbose(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at debug level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& debug(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at silly level.
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& silly(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at emerg level (syslog).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& emerg(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at alert level (syslog).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& alert(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at crit level (syslog).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& crit(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at warning level (syslog).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& warning(const std::string& message, const JsonObject& meta = {});

    /// @brief Log at notice level (syslog).
    /// @param message The log message.
    /// @param meta Optional metadata.
    /// @return Reference to this for chaining.
    ChildLogger& notice(const std::string& message, const JsonObject& meta = {});

    /// @brief Check if a message at the given level would be logged.
    ///
    /// Delegates to the parent Logger's isLevelEnabled().
    ///
    /// @param level The level to check.
    /// @return True if the level is enabled on the parent.
    bool isLevelEnabled(const std::string& level) const;

    /// @brief Create a nested child logger with additional metadata.
    ///
    /// Creates a new ChildLogger with the same parent, but with merged
    /// metadata: childMeta overlaid on top of this child's defaultMeta.
    ///
    /// @param childMeta Additional metadata for the nested child.
    /// @return A new ChildLogger with accumulated metadata.
    ChildLogger child(const JsonObject& childMeta);

private:
    Logger& parent_;
    JsonObject defaultMeta_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/child_logger.hpp>
