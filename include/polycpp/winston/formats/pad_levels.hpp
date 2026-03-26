#pragma once

/// @file formats/pad_levels.hpp
/// @brief PadLevels format -- pads message based on longest level name.

#include <polycpp/winston/format.hpp>
#include <polycpp/winston/colorizer.hpp>
#include <polycpp/winston/level_config.hpp>
#include <map>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the padLevels format.
struct PadLevelsOptions {
    /// Level map to compute padding from. Uses npm levels if empty.
    std::map<std::string, int> levels;
};

/// @brief Pads the message with spaces based on the longest level name.
///
/// Ensures all messages start at the same column regardless of level name length.
/// Uses Colorizer::strip() to measure uncolorized level length.
///
/// @see https://github.com/winstonjs/logform#padlevels
class PadLevelsFormat : public Format {
public:
    /// @brief Construct a padLevels format.
    /// @param options PadLevels options.
    explicit PadLevelsFormat(PadLevelsOptions options = {});

    /// @brief Pad the message based on level name length.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with padded message.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    size_t padding_;
};

/// @brief Factory function for padLevels format.
/// @param options PadLevels options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = padLevels({.levels = LevelConfig::npm().levels});
/// ```
///
/// @see https://github.com/winstonjs/logform#padlevels
std::shared_ptr<Format> padLevels(PadLevelsOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/pad_levels.hpp>
