#pragma once

/// @file detail/format.hpp
/// @brief Inline implementations for Format and CombinedFormat.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {

inline Format::Format(FormatOptions options)
    : options_(std::move(options)) {}

inline CombinedFormat::CombinedFormat(std::vector<std::shared_ptr<Format>> formats)
    : Format()
    , formats_(std::move(formats)) {}

inline std::optional<LogInfo> CombinedFormat::transform(LogInfo info) {
    std::optional<LogInfo> current = std::move(info);
    for (const auto& fmt : formats_) {
        if (!current) {
            return std::nullopt; // Short-circuit: previous format filtered the message
        }
        current = fmt->transform(std::move(*current));
    }
    return current;
}

inline std::shared_ptr<Format> combine(std::vector<std::shared_ptr<Format>> formats) {
    return std::make_shared<CombinedFormat>(std::move(formats));
}

} // namespace winston
} // namespace polycpp
