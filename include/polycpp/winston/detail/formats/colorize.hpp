#pragma once

/// @file detail/formats/colorize.hpp
/// @brief Inline implementation for ColorizeFormat.

#include <polycpp/winston/formats/colorize.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline ColorizeFormat::ColorizeFormat(ColorizeOptions options)
    : opts_(std::move(options)) {
    // Register any additional colors provided in options
    if (!opts_.colors.empty()) {
        Colorizer::addColors(opts_.colors);
    }
}

inline std::optional<LogInfo> ColorizeFormat::transform(LogInfo info) {
    if (opts_.all || opts_.level) {
        info.level = Colorizer::colorize(info.originalLevel, info.level);
    }
    if (opts_.all || opts_.message) {
        info.message = Colorizer::colorize(info.originalLevel, info.message);
    }
    return info;
}

inline std::shared_ptr<Format> colorize(ColorizeOptions options) {
    return std::make_shared<ColorizeFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
