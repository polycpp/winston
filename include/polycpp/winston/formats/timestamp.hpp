#pragma once

/// @file formats/timestamp.hpp
/// @brief Timestamp format -- adds a timestamp to each log entry's metadata.

#include <polycpp/winston/format.hpp>
#include <functional>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the timestamp format.
struct TimestampOptions {
    /// Custom format string (e.g., "YYYY-MM-DD HH:mm:ss").
    /// If empty, uses Date::toISOString() (ISO 8601).
    /// Note: requires polycpp/moment to be available for custom format strings;
    /// otherwise falls back to ISO 8601.
    std::string format;

    /// Custom timestamp function. If set, overrides `format`.
    /// Should return the timestamp string to use.
    std::function<std::string()> fn;

    /// Metadata key to store the timestamp under.
    /// Default: "timestamp".
    std::string alias = "timestamp";
};

/// @brief Adds a timestamp to each log entry.
///
/// Sets `metadata[alias]` to the current timestamp string.
/// Default behavior produces ISO 8601 strings via polycpp::Date::toISOString().
///
/// @see https://github.com/winstonjs/logform#timestamp
class TimestampFormat : public Format {
public:
    /// @brief Construct a timestamp format.
    /// @param options Timestamp options.
    explicit TimestampFormat(TimestampOptions options = {});

    /// @brief Add timestamp to the log entry.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with timestamp added.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    TimestampOptions opts_;
};

/// @brief Factory function for timestamp format.
/// @param options Timestamp options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = timestamp(); // ISO 8601
/// auto fmt = timestamp({.fn = []{ return "custom"; }});
/// auto fmt = timestamp({.alias = "time"});
/// ```
///
/// @see https://github.com/winstonjs/logform#timestamp
std::shared_ptr<Format> timestamp(TimestampOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/timestamp.hpp>
