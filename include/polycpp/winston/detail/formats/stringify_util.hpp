#pragma once

/// @file detail/formats/stringify_util.hpp
/// @brief Shared utility for JSON key sorting (matching safe-stable-stringify).

#include <polycpp/core/json.hpp>
#include <algorithm>
#include <string>
#include <vector>

namespace polycpp {
namespace winston {
namespace formats {
namespace detail {

/// @brief Sort a JsonObject's keys alphabetically (matching safe-stable-stringify).
/// Recursively sorts nested objects and arrays containing objects.
inline JsonValue sortJsonKeys(const JsonValue& val) {
    if (val.isObject()) {
        const auto& obj = val.asObject();
        // Collect keys and sort alphabetically
        std::vector<std::string> keys;
        keys.reserve(obj.size());
        for (const auto& [k, v] : obj) {
            keys.push_back(k);
        }
        std::sort(keys.begin(), keys.end());
        // Rebuild object in sorted order, recursively sorting nested values
        JsonObject sorted;
        for (const auto& k : keys) {
            sorted[k] = sortJsonKeys(obj.at(k));
        }
        return JsonValue(std::move(sorted));
    } else if (val.isArray()) {
        const auto& arr = val.asArray();
        JsonArray sortedArr;
        sortedArr.reserve(arr.size());
        for (const auto& elem : arr) {
            sortedArr.push_back(sortJsonKeys(elem));
        }
        return JsonValue(std::move(sortedArr));
    }
    return val;
}

} // namespace detail
} // namespace formats
} // namespace winston
} // namespace polycpp
