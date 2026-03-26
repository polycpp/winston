#pragma once

/// @file log_info.hpp
/// @brief LogInfo struct -- the central data object flowing through the format pipeline.

#include <polycpp/core/json.hpp>
#include <string>
#include <vector>

namespace polycpp {
namespace winston {

/// @brief Represents a single log entry flowing through the format pipeline.
///
/// Maps to winston's `info` object. Named struct fields replace the
/// Symbol-keyed properties ([LEVEL], [MESSAGE], [SPLAT]) from the JS version.
/// The `metadata` map captures arbitrary extra key-value pairs, preserving
/// the dynamic extensibility of plain JS objects.
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

    /// Final formatted output string (set by finalizing formats: json, simple, printf).
    /// Maps to JS `info[Symbol.for('message')]`.
    std::string formattedMessage;

    /// Printf-style interpolation arguments (e.g., from logger.info('hello %s', 'world')).
    /// Maps to JS `info[Symbol.for('splat')]`.
    std::vector<JsonValue> splat;

    /// Arbitrary extra key-value pairs (service name, request ID, timestamps, etc.).
    /// Replaces all non-Symbol dynamic properties on the JS info object.
    JsonObject metadata;

    /// @brief Default constructor.
    LogInfo() = default;

    /// @brief Construct with level and message.
    ///
    /// Sets both `level` and `originalLevel` to the same value.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    LogInfo(std::string level, std::string message);

    /// @brief Construct with level, message, and splat arguments.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    /// @param splat Printf-style interpolation arguments.
    LogInfo(std::string level, std::string message,
            std::vector<JsonValue> splat);

    /// @brief Construct with level, message, splat, and metadata.
    ///
    /// @param level The log level name.
    /// @param message The log message.
    /// @param splat Printf-style interpolation arguments.
    /// @param metadata Extra key-value pairs.
    LogInfo(std::string level, std::string message,
            std::vector<JsonValue> splat,
            JsonObject metadata);

    /// @brief Merge default metadata into this LogInfo.
    ///
    /// Keys from `defaults` are added only if they don't already exist in `metadata`.
    /// This implements the `defaultMeta` merge behavior from winston's Logger.
    ///
    /// @param defaults Metadata to merge (lower precedence than existing keys).
    void merge(const JsonObject& defaults);

    /// @brief Convert this LogInfo to a JsonValue for serialization.
    ///
    /// Produces a JsonObject with "level", "message", and all metadata keys
    /// at the top level. Used by the `json` format and Http transport.
    ///
    /// @return JsonValue representation of this log entry.
    JsonValue toJsonValue() const;

    /// @brief Check if a metadata key exists.
    /// @param key The metadata key to check.
    /// @return True if the key exists in metadata.
    bool has(const std::string& key) const;

    /// @brief Get a metadata value by key.
    ///
    /// @param key The metadata key to look up.
    /// @return Reference to the value.
    /// @throws std::out_of_range if the key does not exist.
    const JsonValue& get(const std::string& key) const;

    /// @brief Get a metadata value by key, with a default.
    ///
    /// @param key The metadata key to look up.
    /// @param defaultValue Value to return if key is not found.
    /// @return The metadata value, or defaultValue if not found.
    JsonValue get(const std::string& key, JsonValue defaultValue) const;

    /// @brief Set a metadata value.
    /// @param key The metadata key.
    /// @param value The value to set.
    void set(const std::string& key, JsonValue value);

    /// @brief Remove a metadata key.
    /// @param key The metadata key to remove.
    /// @return True if the key existed and was removed.
    bool remove(const std::string& key);
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/log_info.hpp>
