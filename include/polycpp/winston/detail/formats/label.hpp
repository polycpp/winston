#pragma once

/// @file detail/formats/label.hpp
/// @brief Inline implementation for LabelFormat.

#include <polycpp/winston/formats/label.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline LabelFormat::LabelFormat(LabelOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> LabelFormat::transform(LogInfo info) {
    if (opts_.message) {
        info.message = "[" + opts_.label + "] " + info.message;
    } else {
        info.metadata["label"] = JsonValue(opts_.label);
    }
    return info;
}

inline std::shared_ptr<Format> label(LabelOptions options) {
    return std::make_shared<LabelFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
