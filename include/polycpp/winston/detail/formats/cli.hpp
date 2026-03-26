#pragma once

/// @file detail/formats/cli.hpp
/// @brief Inline implementation for CliFormat.

#include <polycpp/winston/formats/cli.hpp>
#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline CliFormat::CliFormat(CliOptions options) {
    combined_ = polycpp::winston::combine({
        colorize(options.colorizeOpts),
        padLevels({.levels = options.levels})
    });
}

inline std::optional<LogInfo> CliFormat::transform(LogInfo info) {
    return combined_->transform(std::move(info));
}

inline std::shared_ptr<Format> cli(CliOptions options) {
    return std::make_shared<CliFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
