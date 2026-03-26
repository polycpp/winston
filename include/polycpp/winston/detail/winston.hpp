#pragma once

/// @file detail/winston.hpp
/// @brief Inline implementations for namespace-level convenience functions.

#include <polycpp/winston/winston.hpp>

namespace polycpp {
namespace winston {

inline std::shared_ptr<Logger> createLogger(LoggerOptions options) {
    return std::make_shared<Logger>(std::move(options));
}

inline Logger& defaultLogger() {
    static Logger instance;
    return instance;
}

inline void configure(const LoggerOptions& opts) {
    defaultLogger().configure(opts);
}

// --- Default logger pass-through functions ---

inline Logger& log(const std::string& level, const std::string& message) {
    return defaultLogger().log(level, message);
}

inline Logger& log(const std::string& level, const std::string& message,
                   const JsonObject& meta) {
    return defaultLogger().log(level, message, meta);
}

inline Logger& log(LogInfo info) {
    return defaultLogger().log(std::move(info));
}

inline Logger& error(const std::string& message, const JsonObject& meta) {
    return defaultLogger().error(message, meta);
}

inline Logger& warn(const std::string& message, const JsonObject& meta) {
    return defaultLogger().warn(message, meta);
}

inline Logger& info(const std::string& message, const JsonObject& meta) {
    return defaultLogger().info(message, meta);
}

inline Logger& http(const std::string& message, const JsonObject& meta) {
    return defaultLogger().http(message, meta);
}

inline Logger& verbose(const std::string& message, const JsonObject& meta) {
    return defaultLogger().verbose(message, meta);
}

inline Logger& debug(const std::string& message, const JsonObject& meta) {
    return defaultLogger().debug(message, meta);
}

inline Logger& silly(const std::string& message, const JsonObject& meta) {
    return defaultLogger().silly(message, meta);
}

inline Logger& add(std::shared_ptr<Transport> transport) {
    return defaultLogger().add(std::move(transport));
}

inline Logger& remove(std::shared_ptr<Transport> transport) {
    return defaultLogger().remove(std::move(transport));
}

inline Logger& clear() {
    return defaultLogger().clear();
}

inline void profile(const std::string& id) {
    defaultLogger().profile(id);
}

inline Profiler startTimer() {
    return defaultLogger().startTimer();
}

inline ChildLogger child(const JsonObject& defaultMeta) {
    return defaultLogger().child(defaultMeta);
}

inline Container& loggers() {
    static Container instance;
    return instance;
}

inline void addColors(const std::map<std::string, std::string>& colors) {
    Colorizer::addColors(colors);
}

} // namespace winston
} // namespace polycpp
