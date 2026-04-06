#pragma once

/// @file detail/formats/json.hpp
/// @brief Inline implementation for JsonFormat.

#include <polycpp/winston/formats/json.hpp>
#include <polycpp/winston/detail/formats/stringify_util.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline JsonFormat::JsonFormat(JsonFormatOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> JsonFormat::transform(LogInfo info) {
    auto jv = info.toJsonValue();
    // Sort keys alphabetically to match npm safe-stable-stringify behavior
    jv = detail::sortJsonKeys(jv);
    info.formattedMessage = polycpp::JSON::stringify(jv, opts_.space);
    return info;
}

inline std::shared_ptr<Format> json(JsonFormatOptions options) {
    return std::make_shared<JsonFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
