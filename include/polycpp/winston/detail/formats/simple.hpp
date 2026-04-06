#pragma once

/// @file detail/formats/simple.hpp
/// @brief Inline implementation for SimpleFormat.

#include <polycpp/winston/formats/simple.hpp>
#include <polycpp/winston/detail/formats/stringify_util.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline SimpleFormat::SimpleFormat() {}

inline std::optional<LogInfo> SimpleFormat::transform(LogInfo info) {
    std::string result = info.level + ": " + info.message;
    if (!info.metadata.empty()) {
        // Sort keys alphabetically to match npm safe-stable-stringify behavior
        auto sorted = detail::sortJsonKeys(JsonValue(info.metadata));
        result += " " + polycpp::JSON::stringify(sorted);
    }
    info.formattedMessage = std::move(result);
    return info;
}

inline std::shared_ptr<Format> simple() {
    return std::make_shared<SimpleFormat>();
}

} // namespace formats
} // namespace winston
} // namespace polycpp
