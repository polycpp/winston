#pragma once

/// @file formats/align.hpp
/// @brief Align format -- prepends a tab character to the message.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Prepends a tab character to the message for alignment.
///
/// @see https://github.com/winstonjs/logform#align
class AlignFormat : public Format {
public:
    /// @brief Construct an align format.
    AlignFormat();

    /// @brief Prepend tab to message.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with tab-prefixed message.
    std::optional<LogInfo> transform(LogInfo info) override;
};

/// @brief Factory function for align format.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = align();
/// ```
///
/// @see https://github.com/winstonjs/logform#align
std::shared_ptr<Format> align();

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/align.hpp>
