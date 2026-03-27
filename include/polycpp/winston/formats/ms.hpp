#pragma once

/// @file formats/ms.hpp
/// @brief Ms format -- adds elapsed time since last log call.

#include <polycpp/winston/format.hpp>
#include <mutex>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Adds elapsed milliseconds since the last log call.
///
/// Sets `metadata["ms"]` to a human-readable elapsed time string
/// (e.g., "+5ms", "+1.5s", "+2m", "+3h", "+1d").
///
/// @see https://github.com/winstonjs/logform#ms
class MsFormat : public Format {
public:
    /// @brief Construct an ms format.
    MsFormat();

    /// @brief Add elapsed time to the log entry.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with ms metadata added.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    double lastTimestamp_;
    mutable std::mutex mutex_;
};

/// @brief Factory function for ms format.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = ms();
/// ```
///
/// @see https://github.com/winstonjs/logform#ms
std::shared_ptr<Format> ms();

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/ms.hpp>
