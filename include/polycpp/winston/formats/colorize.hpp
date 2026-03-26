#pragma once

/// @file formats/colorize.hpp
/// @brief Colorize format -- wraps level/message in ANSI color codes.

#include <polycpp/winston/format.hpp>
#include <polycpp/winston/colorizer.hpp>
#include <map>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the colorize format.
struct ColorizeOptions {
    /// Colorize the level string. Default: true.
    bool level = true;

    /// Colorize both level and message. Default: false.
    /// When true, overrides individual level and message settings.
    bool all = false;

    /// Colorize just the message. Default: false.
    bool message = false;

    /// Additional colors to register (merged into global registry on construction).
    std::map<std::string, std::string> colors;
};

/// @brief Wraps level and/or message in ANSI color codes.
///
/// Uses the global Colorizer registry to look up colors by level name.
/// Colors are applied using polycpp::util::styleText() through the Colorizer.
///
/// @see https://github.com/winstonjs/logform#colorize
class ColorizeFormat : public Format {
public:
    /// @brief Construct a colorize format.
    /// @param options Colorize options.
    explicit ColorizeFormat(ColorizeOptions options = {});

    /// @brief Apply ANSI color codes to level and/or message.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with colorized fields.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    ColorizeOptions opts_;
};

/// @brief Factory function for colorize format.
/// @param options Colorize options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = colorize();                          // level only
/// auto fmt = colorize({.all = true});              // level + message
/// auto fmt = colorize({.level = false, .message = true}); // message only
/// ```
///
/// @see https://github.com/winstonjs/logform#colorize
std::shared_ptr<Format> colorize(ColorizeOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/colorize.hpp>
