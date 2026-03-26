#pragma once

/// @file formats/simple.hpp
/// @brief Simple format -- produces "level: message {metadata}" output.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Produces a simple formatted output string.
///
/// Sets `formattedMessage` to `"${level}: ${message}"`. If metadata is
/// non-empty, appends ` ${JSON.stringify(metadata)}`.
///
/// @see https://github.com/winstonjs/logform#simple
class SimpleFormat : public Format {
public:
    /// @brief Construct a simple format.
    SimpleFormat();

    /// @brief Format the log entry as a simple string.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with formattedMessage set.
    std::optional<LogInfo> transform(LogInfo info) override;
};

/// @brief Factory function for simple format.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = simple();
/// // info: "error", "disk full", metadata: {code: 507}
/// // result: "error: disk full {\"code\":507}"
/// ```
///
/// @see https://github.com/winstonjs/logform#simple
std::shared_ptr<Format> simple();

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/simple.hpp>
