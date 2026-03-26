#pragma once

/// @file detail/formats/metadata.hpp
/// @brief Inline implementation for MetadataFormat.

#include <polycpp/winston/formats/metadata.hpp>
#include <algorithm>

namespace polycpp {
namespace winston {
namespace formats {

inline MetadataFormat::MetadataFormat(MetadataOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> MetadataFormat::transform(LogInfo info) {
    JsonObject nested;
    JsonObject remaining;

    if (!opts_.fillWith.empty()) {
        // Move only the specified keys into the nested object
        for (const auto& [key, value] : info.metadata) {
            auto found = std::find(opts_.fillWith.begin(), opts_.fillWith.end(), key);
            if (found != opts_.fillWith.end()) {
                nested[key] = value;
            } else {
                remaining[key] = value;
            }
        }
    } else if (!opts_.fillExcept.empty()) {
        // Move all keys except the specified ones
        for (const auto& [key, value] : info.metadata) {
            auto found = std::find(opts_.fillExcept.begin(), opts_.fillExcept.end(), key);
            if (found != opts_.fillExcept.end()) {
                remaining[key] = value; // Keep at top level
            } else {
                nested[key] = value; // Move to nested
            }
        }
    } else {
        // Move all metadata into nested object
        nested = std::move(info.metadata);
    }

    info.metadata = std::move(remaining);
    if (!nested.empty()) {
        info.metadata[opts_.key] = JsonValue(std::move(nested));
    }

    return info;
}

inline std::shared_ptr<Format> metadata(MetadataOptions options) {
    return std::make_shared<MetadataFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
