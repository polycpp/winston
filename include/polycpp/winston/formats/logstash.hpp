#pragma once

/// @file formats/logstash.hpp
/// @brief Logstash format -- produces Logstash v1 JSON output.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Produces Logstash v1 JSON format output.
///
/// Sets `formattedMessage` to a JSON string with the structure:
/// `{"@message": msg, "@timestamp": ts, "@fields": metadata, "level": level}`
///
/// If `metadata["timestamp"]` exists, uses that; otherwise generates a
/// current ISO 8601 timestamp.
///
/// @see https://github.com/winstonjs/logform#logstash
class LogstashFormat : public Format {
public:
    /// @brief Construct a logstash format.
    LogstashFormat();

    /// @brief Format the log entry as Logstash v1 JSON.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with formattedMessage set.
    std::optional<LogInfo> transform(LogInfo info) override;
};

/// @brief Factory function for logstash format.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = logstash();
/// ```
///
/// @see https://github.com/winstonjs/logform#logstash
std::shared_ptr<Format> logstash();

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/logstash.hpp>
