#pragma once

/// @file detail/formats/pad_levels.hpp
/// @brief Inline implementation for PadLevelsFormat.

#include <polycpp/winston/formats/pad_levels.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline PadLevelsFormat::PadLevelsFormat(PadLevelsOptions options)
    : padding_(0) {
    auto levels = options.levels.empty() ? LevelConfig::npm().levels : options.levels;

    // Find the longest level name
    for (const auto& [name, priority] : levels) {
        if (name.size() > padding_) {
            padding_ = name.size();
        }
    }
}

inline std::optional<LogInfo> PadLevelsFormat::transform(LogInfo info) {
    // Use Colorizer::strip to measure the uncolorized level length
    // (since colorize may have added ANSI codes)
    size_t levelLen = Colorizer::strip(info.level).size();
    size_t pad = (padding_ > levelLen) ? (padding_ - levelLen + 1) : 1;
    info.message = std::string(pad, ' ') + info.message;
    return info;
}

inline std::shared_ptr<Format> padLevels(PadLevelsOptions options) {
    return std::make_shared<PadLevelsFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
