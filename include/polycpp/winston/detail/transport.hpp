#pragma once

/// @file detail/transport.hpp
/// @brief Inline implementations for Transport.

#include <polycpp/winston/transport.hpp>

namespace polycpp {
namespace winston {

inline Transport::Transport(TransportOptions options)
    : level(std::move(options.level))
    , silent(options.silent)
    , handleExceptions(options.handleExceptions)
    , handleRejections(options.handleRejections)
    , format(std::move(options.format)) {}

inline void Transport::setLevels(const LevelConfig* levels) {
    levels_ = levels;
}

inline bool Transport::accept(const LogInfo& info) const {
    // If no per-transport level set, accept all (logger-level filtering
    // has already been applied)
    if (level.empty()) {
        return true;
    }
    // If no level config pointer, accept all (defensive)
    if (!levels_) {
        return true;
    }
    return levels_->isLevelEnabled(level, info.originalLevel);
}

inline void Transport::write(LogInfo info) {
    // Silent check
    if (silent) {
        return;
    }

    // Level filtering
    if (!accept(info)) {
        return;
    }

    // Apply per-transport format (if any)
    if (format) {
        auto result = format->transform(std::move(info));
        if (!result) {
            return; // Format filtered the message
        }
        info = std::move(*result);
    }

    // Delegate to subclass
    log(info);

    // Emit "logged" event
    emit(event::Logged);
}

} // namespace winston
} // namespace polycpp
