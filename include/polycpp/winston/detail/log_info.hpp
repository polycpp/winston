#pragma once

/// @file detail/log_info.hpp
/// @brief Inline implementations for LogInfo.

#include <polycpp/winston/log_info.hpp>

namespace polycpp {
namespace winston {

// NOTE: Member initialization order matters here. 'level' is declared before
// 'originalLevel' in the struct, so it is initialized first via move, then
// 'originalLevel' is copy-initialized from the already-moved-into 'this->level'.
// Do NOT reorder the member declarations in LogInfo.
inline LogInfo::LogInfo(std::string level, std::string message)
    : level(std::move(level))
    , originalLevel(this->level)
    , message(std::move(message)) {}

inline LogInfo::LogInfo(std::string level, std::string message,
                        std::vector<JsonValue> splat)
    : level(std::move(level))
    , originalLevel(this->level)
    , message(std::move(message))
    , splat(std::move(splat)) {}

inline LogInfo::LogInfo(std::string level, std::string message,
                        std::vector<JsonValue> splat,
                        JsonObject metadata)
    : level(std::move(level))
    , originalLevel(this->level)
    , message(std::move(message))
    , splat(std::move(splat))
    , metadata(std::move(metadata)) {}

inline void LogInfo::merge(const JsonObject& defaults) {
    for (const auto& [key, value] : defaults) {
        // Only add if key doesn't already exist (log-call metadata takes precedence)
        if (metadata.find(key) == metadata.end()) {
            metadata[key] = value;
        }
    }
}

inline JsonValue LogInfo::toJsonValue() const {
    JsonObject obj;
    obj["level"] = JsonValue(level);
    obj["message"] = JsonValue(message);
    // Merge all metadata at the top level (matches JS behavior where
    // info = { level, message, ...metadata })
    for (const auto& [key, value] : metadata) {
        obj[key] = value;
    }
    return JsonValue(std::move(obj));
}

inline bool LogInfo::has(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

inline const JsonValue& LogInfo::get(const std::string& key) const {
    return metadata.at(key);
}

inline JsonValue LogInfo::get(const std::string& key, JsonValue defaultValue) const {
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return defaultValue;
}

inline void LogInfo::set(const std::string& key, JsonValue value) {
    metadata[key] = std::move(value);
}

inline bool LogInfo::remove(const std::string& key) {
    return metadata.erase(key) > 0;
}

} // namespace winston
} // namespace polycpp
