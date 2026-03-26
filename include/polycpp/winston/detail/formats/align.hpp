#pragma once

/// @file detail/formats/align.hpp
/// @brief Inline implementation for AlignFormat.

#include <polycpp/winston/formats/align.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline AlignFormat::AlignFormat() {}

inline std::optional<LogInfo> AlignFormat::transform(LogInfo info) {
    info.message = "\t" + info.message;
    return info;
}

inline std::shared_ptr<Format> align() {
    return std::make_shared<AlignFormat>();
}

} // namespace formats
} // namespace winston
} // namespace polycpp
