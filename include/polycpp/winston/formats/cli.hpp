#pragma once

/// @file formats/cli.hpp
/// @brief CLI format -- composite of colorize + padLevels.

#include <polycpp/winston/format.hpp>
#include <polycpp/winston/formats/colorize.hpp>
#include <polycpp/winston/formats/pad_levels.hpp>
#include <map>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the cli format.
struct CliOptions {
    /// Level map for padLevels. Default: npm levels.
    std::map<std::string, int> levels;

    /// Colorize options (passed through to colorize format).
    ColorizeOptions colorizeOpts;
};

/// @brief Composite format combining colorize and padLevels.
///
/// Equivalent to `combine({colorize(opts), padLevels({.levels = levels})})`.
/// Produces CLI-friendly output with colorized levels and aligned messages.
///
/// @see https://github.com/winstonjs/logform#cli
class CliFormat : public Format {
public:
    /// @brief Construct a cli format.
    /// @param options CLI options.
    explicit CliFormat(CliOptions options = {});

    /// @brief Apply colorize + padLevels pipeline.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    std::shared_ptr<Format> combined_;
};

/// @brief Factory function for cli format.
/// @param options CLI options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = cli({.levels = LevelConfig::npm().levels});
/// ```
///
/// @see https://github.com/winstonjs/logform#cli
std::shared_ptr<Format> cli(CliOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/cli.hpp>
