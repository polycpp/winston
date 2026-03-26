#pragma once

/// @file detail/formats/uncolorize.hpp
/// @brief Inline implementation for UncolorizeFormat.

#include <polycpp/winston/formats/uncolorize.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline UncolorizeFormat::UncolorizeFormat(UncolorizeOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> UncolorizeFormat::transform(LogInfo info) {
    if (opts_.level) {
        info.level = Colorizer::strip(info.level);
    }
    if (opts_.message) {
        info.message = Colorizer::strip(info.message);
    }
    if (opts_.raw) {
        info.formattedMessage = Colorizer::strip(info.formattedMessage);
    }
    return info;
}

inline std::shared_ptr<Format> uncolorize(UncolorizeOptions options) {
    return std::make_shared<UncolorizeFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
