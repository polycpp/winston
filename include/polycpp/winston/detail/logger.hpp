#pragma once

/// @file detail/logger.hpp
/// @brief Inline implementations for Logger.

#include <polycpp/winston/logger.hpp>
#include <polycpp/winston/profiler.hpp>
#include <polycpp/winston/child_logger.hpp>
#include <polycpp/core/date.hpp>
#include <algorithm>

namespace polycpp {
namespace winston {

inline Logger::Logger(LoggerOptions options)
    : level_(std::move(options.level))
    , levels_(std::move(options.levels))
    , format_(std::move(options.format))
    , defaultMeta_(std::move(options.defaultMeta))
    , silent_(options.silent)
    , exitOnError_(std::move(options.exitOnError)) {
    // Add each transport via add() to wire up event forwarding
    for (auto& transport : options.transports) {
        add(std::move(transport));
    }
}

inline Logger& Logger::log(LogInfo info) {
    // 1. Silent check
    if (silent_) {
        return *this;
    }

    // 2. Set originalLevel if empty
    if (info.originalLevel.empty()) {
        info.originalLevel = info.level;
    }

    // 3. Check level validity -- if message level is not in levels_, return
    if (levels_.levels.find(info.originalLevel) == levels_.levels.end()) {
        return *this;
    }

    // 4. Check level filtering
    if (!levels_.isLevelEnabled(level_, info.originalLevel)) {
        return *this;
    }

    // 5. Merge defaultMeta into info.metadata (info.metadata takes precedence)
    info.merge(defaultMeta_);

    // 6. Dispatch
    write(std::move(info));
    return *this;
}

inline Logger& Logger::log(const std::string& level, const std::string& message) {
    LogInfo info(level, message);
    return log(std::move(info));
}

inline Logger& Logger::log(const std::string& level, const std::string& message,
                            const JsonObject& meta) {
    LogInfo info;
    info.level = level;
    info.originalLevel = level;
    info.message = message;
    info.metadata = meta;
    return log(std::move(info));
}

// --- Fixed npm level methods ---

inline Logger& Logger::error(const std::string& message, const JsonObject& meta) {
    return log("error", message, meta);
}

inline Logger& Logger::warn(const std::string& message, const JsonObject& meta) {
    return log("warn", message, meta);
}

inline Logger& Logger::info(const std::string& message, const JsonObject& meta) {
    return log("info", message, meta);
}

inline Logger& Logger::http(const std::string& message, const JsonObject& meta) {
    return log("http", message, meta);
}

inline Logger& Logger::verbose(const std::string& message, const JsonObject& meta) {
    return log("verbose", message, meta);
}

inline Logger& Logger::debug(const std::string& message, const JsonObject& meta) {
    return log("debug", message, meta);
}

inline Logger& Logger::silly(const std::string& message, const JsonObject& meta) {
    return log("silly", message, meta);
}

// --- Fixed syslog level methods ---

inline Logger& Logger::emerg(const std::string& message, const JsonObject& meta) {
    return log("emerg", message, meta);
}

inline Logger& Logger::alert(const std::string& message, const JsonObject& meta) {
    return log("alert", message, meta);
}

inline Logger& Logger::crit(const std::string& message, const JsonObject& meta) {
    return log("crit", message, meta);
}

inline Logger& Logger::warning(const std::string& message, const JsonObject& meta) {
    return log("warning", message, meta);
}

inline Logger& Logger::notice(const std::string& message, const JsonObject& meta) {
    return log("notice", message, meta);
}

// --- Transport management ---

inline Logger& Logger::add(std::shared_ptr<Transport> transport) {
    // Set the transport's level config
    transport->setLevels(&levels_);

    // Wire up error event forwarding (extract the first arg to avoid double-wrapping)
    transport->on("error", [this](const std::vector<std::any>& args) {
        if (!args.empty()) {
            this->emit("error", args[0]);
        }
    });
    auto listenerId = transport->lastListenerId();
    errorListenerIds_[transport.get()] = listenerId;

    transports_.push_back(std::move(transport));
    return *this;
}

inline Logger& Logger::remove(std::shared_ptr<Transport> transport) {
    // Remove event forwarding listener
    auto it = errorListenerIds_.find(transport.get());
    if (it != errorListenerIds_.end()) {
        transport->removeListener("error", it->second);
        errorListenerIds_.erase(it);
    }

    // Remove from transports vector
    auto tIt = std::find(transports_.begin(), transports_.end(), transport);
    if (tIt != transports_.end()) {
        transports_.erase(tIt);
    }
    return *this;
}

inline Logger& Logger::clear() {
    // Remove all error forwarding listeners
    for (auto& transport : transports_) {
        auto it = errorListenerIds_.find(transport.get());
        if (it != errorListenerIds_.end()) {
            transport->removeListener("error", it->second);
        }
    }
    errorListenerIds_.clear();
    transports_.clear();
    return *this;
}

inline void Logger::close() {
    clear();
    emit("close");
}

// --- Configuration ---

inline void Logger::configure(const LoggerOptions& options) {
    clear();
    profilers_.clear();
    level_ = options.level;
    levels_ = options.levels;
    format_ = options.format;
    defaultMeta_ = options.defaultMeta;
    silent_ = options.silent;
    exitOnError_ = options.exitOnError;

    // Re-add transports via add() for event forwarding
    for (const auto& transport : options.transports) {
        add(transport);
    }
}

// --- Level checking ---

inline bool Logger::isLevelEnabled(const std::string& level) const {
    return levels_.isLevelEnabled(level_, level);
}

inline bool Logger::isErrorEnabled() const {
    return isLevelEnabled("error");
}

inline bool Logger::isWarnEnabled() const {
    return isLevelEnabled("warn");
}

inline bool Logger::isInfoEnabled() const {
    return isLevelEnabled("info");
}

inline bool Logger::isHttpEnabled() const {
    return isLevelEnabled("http");
}

inline bool Logger::isVerboseEnabled() const {
    return isLevelEnabled("verbose");
}

inline bool Logger::isDebugEnabled() const {
    return isLevelEnabled("debug");
}

inline bool Logger::isSillyEnabled() const {
    return isLevelEnabled("silly");
}

// --- Profiling ---

inline void Logger::profile(const std::string& id) {
    auto it = profilers_.find(id);
    if (it != profilers_.end()) {
        // Second call: compute duration and log
        double durationMs = polycpp::Date::now() - it->second;
        profilers_.erase(it);
        LogInfo info;
        info.level = "info";
        info.originalLevel = "info";
        info.message = id;
        info.metadata["durationMs"] = JsonValue(durationMs);
        log(std::move(info));
    } else {
        // First call: start timer
        profilers_[id] = polycpp::Date::now();
    }
}

inline void Logger::profile(const std::string& id, const JsonObject& meta) {
    auto it = profilers_.find(id);
    if (it != profilers_.end()) {
        // Second call: compute duration and log with meta
        double durationMs = polycpp::Date::now() - it->second;
        profilers_.erase(it);
        LogInfo info;
        info.level = "info";
        info.originalLevel = "info";
        info.message = id;
        info.metadata = meta;
        info.metadata["durationMs"] = JsonValue(durationMs);
        log(std::move(info));
    } else {
        // First call: start timer
        profilers_[id] = polycpp::Date::now();
    }
}

inline Profiler Logger::startTimer() {
    return Profiler(*this);
}

// --- Child loggers ---

inline ChildLogger Logger::child(const JsonObject& defaultMeta) {
    return ChildLogger(*this, defaultMeta);
}

// --- Accessors ---

inline const std::string& Logger::level() const {
    return level_;
}

inline void Logger::setLevel(const std::string& lvl) {
    level_ = lvl;
}

inline const LevelConfig& Logger::levels() const {
    return levels_;
}

inline const std::vector<std::shared_ptr<Transport>>& Logger::transports() const {
    return transports_;
}

inline bool Logger::silent() const {
    return silent_;
}

inline void Logger::setSilent(bool s) {
    silent_ = s;
}

inline const JsonObject& Logger::defaultMeta() const {
    return defaultMeta_;
}

inline void Logger::setDefaultMeta(const JsonObject& meta) {
    defaultMeta_ = meta;
}

inline const std::shared_ptr<Format>& Logger::format() const {
    return format_;
}

// --- Protected dispatch ---

inline void Logger::write(LogInfo info) {
    // Apply logger-level format
    if (format_) {
        auto result = format_->transform(std::move(info));
        if (!result) {
            return; // Format filtered the message
        }
        info = std::move(*result);
    }

    // Dispatch to all transports
    for (auto& transport : transports_) {
        transport->write(info);
    }
}

} // namespace winston
} // namespace polycpp
