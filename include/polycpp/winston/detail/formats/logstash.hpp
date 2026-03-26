#pragma once

/// @file detail/formats/logstash.hpp
/// @brief Inline implementation for LogstashFormat.

#include <polycpp/winston/formats/logstash.hpp>
#include <polycpp/core/json.hpp>
#include <polycpp/core/date.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline LogstashFormat::LogstashFormat() {}

inline std::optional<LogInfo> LogstashFormat::transform(LogInfo info) {
    JsonObject obj;
    obj["@message"] = JsonValue(info.message);

    // Use existing timestamp from metadata if available, otherwise generate one
    auto tsIt = info.metadata.find("timestamp");
    if (tsIt != info.metadata.end() && tsIt->second.isString()) {
        obj["@timestamp"] = tsIt->second;
    } else {
        obj["@timestamp"] = JsonValue(polycpp::Date(polycpp::Date::now()).toISOString());
    }

    obj["@fields"] = JsonValue(info.metadata);
    obj["level"] = JsonValue(info.originalLevel);

    info.formattedMessage = polycpp::JSON::stringify(JsonValue(std::move(obj)));
    return info;
}

inline std::shared_ptr<Format> logstash() {
    return std::make_shared<LogstashFormat>();
}

} // namespace formats
} // namespace winston
} // namespace polycpp
