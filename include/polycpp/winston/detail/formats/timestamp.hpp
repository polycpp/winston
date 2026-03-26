#pragma once

/// @file detail/formats/timestamp.hpp
/// @brief Inline implementation for TimestampFormat.

#include <polycpp/winston/formats/timestamp.hpp>
#include <polycpp/core/date.hpp>

#if __has_include(<polycpp/moment/moment.hpp>)
#include <polycpp/moment/moment.hpp>
#define POLYCPP_WINSTON_HAS_MOMENT 1
#else
#define POLYCPP_WINSTON_HAS_MOMENT 0
#endif

namespace polycpp {
namespace winston {
namespace formats {

inline TimestampFormat::TimestampFormat(TimestampOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> TimestampFormat::transform(LogInfo info) {
    std::string ts;

    if (opts_.fn) {
        ts = opts_.fn();
    }
#if POLYCPP_WINSTON_HAS_MOMENT
    else if (!opts_.format.empty()) {
        ts = polycpp::moment::Moment(polycpp::Date::now()).format(opts_.format);
    }
#endif
    else {
        ts = polycpp::Date(polycpp::Date::now()).toISOString();
    }

    info.metadata[opts_.alias] = JsonValue(std::move(ts));
    return info;
}

inline std::shared_ptr<Format> timestamp(TimestampOptions options) {
    return std::make_shared<TimestampFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
