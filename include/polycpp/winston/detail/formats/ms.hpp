#pragma once

/// @file detail/formats/ms.hpp
/// @brief Inline implementation for MsFormat.

#include <polycpp/winston/formats/ms.hpp>
#include <polycpp/core/date.hpp>
#include <polycpp/core/number.hpp>
#include <cmath>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

namespace detail {

/// @brief Convert milliseconds to a human-readable string.
///
/// Produces strings like "+5ms", "+1.5s", "+2m", "+3h", "+1d".
///
/// @param ms Milliseconds value.
/// @return Human-readable string prefixed with "+".
inline std::string msToString(double ms) {
    std::string result;
    if (ms >= 86400000.0) {
        result = polycpp::Number::toString(std::floor(ms / 86400000.0)) + "d";
    } else if (ms >= 3600000.0) {
        result = polycpp::Number::toString(std::floor(ms / 3600000.0)) + "h";
    } else if (ms >= 60000.0) {
        result = polycpp::Number::toString(std::floor(ms / 60000.0)) + "m";
    } else if (ms >= 1000.0) {
        // Show one decimal for seconds
        double secs = std::floor(ms / 100.0) / 10.0;
        result = polycpp::Number::toString(secs) + "s";
    } else {
        result = polycpp::Number::toString(std::floor(ms)) + "ms";
    }
    return "+" + result;
}

} // namespace detail

inline MsFormat::MsFormat()
    : lastTimestamp_(polycpp::Date::now()) {}

inline std::optional<LogInfo> MsFormat::transform(LogInfo info) {
    double now = polycpp::Date::now();
    double elapsed = now - lastTimestamp_;
    lastTimestamp_ = now;

    info.metadata["ms"] = JsonValue(detail::msToString(elapsed));
    return info;
}

inline std::shared_ptr<Format> ms() {
    return std::make_shared<MsFormat>();
}

} // namespace formats
} // namespace winston
} // namespace polycpp
