#pragma once

/// @file formats/splat.hpp
/// @brief Splat format -- runtime printf-style interpolation using LogInfo::splat.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Performs printf-style interpolation of format specifiers in the message.
///
/// Iterates `%s`, `%d`, `%i`, `%f`, `%j`, `%o`, `%O`, `%%` tokens in
/// `info.message` against values in `info.splat`. Extra object-typed splat
/// values after all specifiers are consumed are merged into `info.metadata`.
///
/// @see https://github.com/winstonjs/logform#splat
class SplatFormat : public Format {
public:
    /// @brief Construct a splat format.
    SplatFormat();

    /// @brief Interpolate format specifiers with splat arguments.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with interpolated message.
    std::optional<LogInfo> transform(LogInfo info) override;
};

/// @brief Factory function for splat format.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = splat();
/// // info.message = "hello %s", info.splat = ["world"]
/// // result: info.message = "hello world"
/// ```
///
/// @see https://github.com/winstonjs/logform#splat
std::shared_ptr<Format> splat();

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/splat.hpp>
