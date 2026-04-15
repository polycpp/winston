#pragma once

/// @file detail/container.hpp
/// @brief Inline implementations for Container.

#include <polycpp/winston/container.hpp>

namespace polycpp {
namespace winston {

inline Container::Container(LoggerOptions defaults)
    : defaults_(std::move(defaults)) {}

inline std::shared_ptr<Logger> Container::get(const std::string& id,
                                                const LoggerOptions& opts) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Return existing logger if present
    auto it = loggers_.find(id);
    if (it != loggers_.end()) {
        return it->second;
    }

    // Merge options: use opts fields if non-default, else fall back to defaults_
    LoggerOptions merged = defaults_;

    // Override with user-provided opts if they differ from struct defaults
    if (!opts.transports.empty()) {
        merged.transports = opts.transports;
    }
    if (!opts.level.empty() && opts.level != "info") {
        merged.level = opts.level;
    } else if (opts.transports.empty() && opts.level == "info" && defaults_.level != "info") {
        // Keep defaults_.level if user didn't explicitly override
    } else if (!opts.level.empty()) {
        merged.level = opts.level;
    }
    if (opts.format) {
        merged.format = opts.format;
    }
    if (opts.silent) {
        merged.silent = opts.silent;
    }
    if (!opts.defaultMeta.empty()) {
        merged.defaultMeta = opts.defaultMeta;
    }

    auto logger = std::make_shared<Logger>(std::move(merged));

    // Auto-remove on close event
    logger->on(event::Close, [this, id]() {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.erase(id);
    });

    loggers_[id] = logger;
    return logger;
}

inline std::shared_ptr<Logger> Container::add(const std::string& id,
                                                const LoggerOptions& opts) {
    return get(id, opts);
}

inline bool Container::has(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return loggers_.count(id) > 0;
}

inline void Container::close(const std::string& id) {
    std::shared_ptr<Logger> logger;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = loggers_.find(id);
        if (it != loggers_.end()) {
            logger = it->second;
            loggers_.erase(it);
        }
    }
    if (logger) {
        logger->close();
    }
}

inline void Container::close() {
    // Collect all loggers first (close() may trigger auto-removal via events)
    std::vector<std::shared_ptr<Logger>> all;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        all.reserve(loggers_.size());
        for (auto& [id, logger] : loggers_) {
            all.push_back(logger);
        }
        loggers_.clear();
    }
    for (auto& logger : all) {
        logger->close();
    }
}

} // namespace winston
} // namespace polycpp
