#pragma once

/// @file formats/json.hpp
/// @brief JSON format -- serializes LogInfo to JSON string.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the json format.
struct JsonFormatOptions {
    /// Number of spaces for indentation. 0 means compact.
    int space = 0;
};

/// @brief Serializes the LogInfo to a JSON string.
///
/// Sets `formattedMessage` to JSON::stringify of the info as a JsonValue.
/// The output includes "level", "message", and all metadata keys at the
/// top level (matching JS behavior where info is a flat object).
///
/// @see https://github.com/winstonjs/logform#json
class JsonFormat : public Format {
public:
    /// @brief Construct a json format.
    /// @param options JSON format options.
    explicit JsonFormat(JsonFormatOptions options = {});

    /// @brief Serialize the log entry to JSON.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with formattedMessage set to JSON.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    JsonFormatOptions opts_;
};

/// @brief Factory function for json format.
/// @param options JSON format options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = json();            // compact
/// auto fmt = json({.space = 2}); // pretty-printed
/// ```
///
/// @see https://github.com/winstonjs/logform#json
std::shared_ptr<Format> json(JsonFormatOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/json.hpp>
