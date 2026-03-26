#pragma once

/// @file formats/uncolorize.hpp
/// @brief Uncolorize format -- strips ANSI escape codes from log fields.

#include <polycpp/winston/format.hpp>
#include <polycpp/winston/colorizer.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the uncolorize format.
struct UncolorizeOptions {
    /// Strip ANSI codes from level. Default: true.
    bool level = true;

    /// Strip ANSI codes from message. Default: true.
    bool message = true;

    /// If true, also strip from formattedMessage. Default: false.
    bool raw = false;
};

/// @brief Strips ANSI escape codes from level, message, and optionally formattedMessage.
///
/// Uses polycpp::util::stripVTControlCharacters() via Colorizer::strip().
///
/// @see https://github.com/winstonjs/logform#uncolorize
class UncolorizeFormat : public Format {
public:
    /// @brief Construct an uncolorize format.
    /// @param options Uncolorize options.
    explicit UncolorizeFormat(UncolorizeOptions options = {});

    /// @brief Strip ANSI codes from selected fields.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with ANSI codes stripped.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    UncolorizeOptions opts_;
};

/// @brief Factory function for uncolorize format.
/// @param options Uncolorize options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = uncolorize();
/// auto fmt = uncolorize({.raw = true});
/// ```
///
/// @see https://github.com/winstonjs/logform#uncolorize
std::shared_ptr<Format> uncolorize(UncolorizeOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/uncolorize.hpp>
