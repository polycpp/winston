#pragma once

/// @file detail/formats/errors.hpp
/// @brief Inline implementation for ErrorsFormat.

#include <polycpp/winston/formats/errors.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline ErrorsFormat::ErrorsFormat(ErrorsOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> ErrorsFormat::transform(LogInfo info) {
    auto it = info.metadata.find("error");
    if (it == info.metadata.end()) {
        return info;
    }

    // Check if the error value is an object with expected fields
    if (!it->second.isObject()) {
        return info;
    }

    const auto errorObj = it->second.asObject();  // copy to avoid invalidation

    // Extract error message if info.message is empty
    auto msgIt = errorObj.find("message");
    if (msgIt != errorObj.end() && msgIt->second.isString() && info.message.empty()) {
        info.message = msgIt->second.asString();
    }

    // Extract name
    auto nameIt = errorObj.find("name");
    if (nameIt != errorObj.end() && nameIt->second.isString()) {
        info.metadata["errorName"] = nameIt->second;
    }

    // Extract stack (only if enabled)
    if (opts_.stack) {
        auto stackIt = errorObj.find("stack");
        if (stackIt != errorObj.end() && stackIt->second.isString()) {
            info.metadata["stack"] = stackIt->second;
        }
    }

    // Extract code if present
    auto codeIt = errorObj.find("code");
    if (codeIt != errorObj.end()) {
        info.metadata["errorCode"] = codeIt->second;
    }

    return info;
}

inline std::shared_ptr<Format> errors(ErrorsOptions options) {
    return std::make_shared<ErrorsFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
