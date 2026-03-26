#pragma once

/// @file detail/profiler.hpp
/// @brief Inline implementations for Profiler.

#include <polycpp/winston/profiler.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/core/date.hpp>

namespace polycpp {
namespace winston {

inline Profiler::Profiler(Logger& logger)
    : logger_(logger)
    , start_(polycpp::Date::now()) {}

inline void Profiler::done(const std::string& message) {
    double durationMs = polycpp::Date::now() - start_;
    LogInfo info;
    info.level = "info";
    info.originalLevel = "info";
    info.message = message;
    info.metadata["durationMs"] = JsonValue(durationMs);
    logger_.log(std::move(info));
}

inline void Profiler::done(LogInfo info) {
    double durationMs = polycpp::Date::now() - start_;
    if (info.level.empty()) {
        info.level = "info";
    }
    if (info.originalLevel.empty()) {
        info.originalLevel = info.level;
    }
    info.metadata["durationMs"] = JsonValue(durationMs);
    logger_.log(std::move(info));
}

} // namespace winston
} // namespace polycpp
