#pragma once

/// @file detail/formats/printf.hpp
/// @brief Inline implementation for PrintfFormat.

#include <polycpp/winston/formats/printf.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline PrintfFormat::PrintfFormat(std::function<std::string(const LogInfo&)> fn)
    : fn_(std::move(fn)) {}

inline std::optional<LogInfo> PrintfFormat::transform(LogInfo info) {
    info.formattedMessage = fn_(info);
    return info;
}

inline std::shared_ptr<Format> printf(std::function<std::string(const LogInfo&)> fn) {
    return std::make_shared<PrintfFormat>(std::move(fn));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
