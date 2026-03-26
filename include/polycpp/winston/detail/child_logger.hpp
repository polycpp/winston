#pragma once

/// @file detail/child_logger.hpp
/// @brief Inline implementations for ChildLogger.

#include <polycpp/winston/child_logger.hpp>
#include <polycpp/winston/logger.hpp>

namespace polycpp {
namespace winston {

inline ChildLogger::ChildLogger(Logger& parent, const JsonObject& defaultMeta)
    : parent_(parent)
    , defaultMeta_(defaultMeta) {}

inline ChildLogger& ChildLogger::log(LogInfo info) {
    // Merge child's defaultMeta into info.metadata
    // (per-call metadata takes precedence over child defaults)
    info.merge(defaultMeta_);
    // Forward to parent (parent will merge its own defaultMeta)
    parent_.log(std::move(info));
    return *this;
}

inline ChildLogger& ChildLogger::log(const std::string& level, const std::string& message) {
    LogInfo info(level, message);
    return log(std::move(info));
}

inline ChildLogger& ChildLogger::log(const std::string& level, const std::string& message,
                                      const JsonObject& meta) {
    LogInfo info;
    info.level = level;
    info.originalLevel = level;
    info.message = message;
    info.metadata = meta;
    return log(std::move(info));
}

// --- Fixed npm level methods ---

inline ChildLogger& ChildLogger::error(const std::string& message, const JsonObject& meta) {
    return log("error", message, meta);
}

inline ChildLogger& ChildLogger::warn(const std::string& message, const JsonObject& meta) {
    return log("warn", message, meta);
}

inline ChildLogger& ChildLogger::info(const std::string& message, const JsonObject& meta) {
    return log("info", message, meta);
}

inline ChildLogger& ChildLogger::http(const std::string& message, const JsonObject& meta) {
    return log("http", message, meta);
}

inline ChildLogger& ChildLogger::verbose(const std::string& message, const JsonObject& meta) {
    return log("verbose", message, meta);
}

inline ChildLogger& ChildLogger::debug(const std::string& message, const JsonObject& meta) {
    return log("debug", message, meta);
}

inline ChildLogger& ChildLogger::silly(const std::string& message, const JsonObject& meta) {
    return log("silly", message, meta);
}

// --- Fixed syslog level methods ---

inline ChildLogger& ChildLogger::emerg(const std::string& message, const JsonObject& meta) {
    return log("emerg", message, meta);
}

inline ChildLogger& ChildLogger::alert(const std::string& message, const JsonObject& meta) {
    return log("alert", message, meta);
}

inline ChildLogger& ChildLogger::crit(const std::string& message, const JsonObject& meta) {
    return log("crit", message, meta);
}

inline ChildLogger& ChildLogger::warning(const std::string& message, const JsonObject& meta) {
    return log("warning", message, meta);
}

inline ChildLogger& ChildLogger::notice(const std::string& message, const JsonObject& meta) {
    return log("notice", message, meta);
}

// --- Level checking ---

inline bool ChildLogger::isLevelEnabled(const std::string& level) const {
    return parent_.isLevelEnabled(level);
}

// --- Nested child ---

inline ChildLogger ChildLogger::child(const JsonObject& childMeta) {
    // Merge childMeta on top of this child's defaultMeta
    JsonObject merged = defaultMeta_;
    for (const auto& [key, value] : childMeta) {
        merged[key] = value;
    }
    return ChildLogger(parent_, merged);
}

} // namespace winston
} // namespace polycpp
