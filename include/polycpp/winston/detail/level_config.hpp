#pragma once

/// @file detail/level_config.hpp
/// @brief Inline implementations for LevelConfig presets.

#include <polycpp/winston/level_config.hpp>

namespace polycpp {
namespace winston {

inline LevelConfig LevelConfig::npm() {
    return LevelConfig{
        .levels = {
            {"error", 0}, {"warn", 1}, {"info", 2}, {"http", 3},
            {"verbose", 4}, {"debug", 5}, {"silly", 6}
        },
        .colors = {
            {"error", "red"}, {"warn", "yellow"}, {"info", "green"},
            {"http", "green"}, {"verbose", "cyan"}, {"debug", "blue"},
            {"silly", "magenta"}
        }
    };
}

inline LevelConfig LevelConfig::syslog() {
    return LevelConfig{
        .levels = {
            {"emerg", 0}, {"alert", 1}, {"crit", 2}, {"error", 3},
            {"warning", 4}, {"notice", 5}, {"info", 6}, {"debug", 7}
        },
        .colors = {
            {"emerg", "red"}, {"alert", "yellow"}, {"crit", "red"},
            {"error", "red"}, {"warning", "red"}, {"notice", "yellow"},
            {"info", "green"}, {"debug", "blue"}
        }
    };
}

inline LevelConfig LevelConfig::cli() {
    return LevelConfig{
        .levels = {
            {"error", 0}, {"warn", 1}, {"help", 2}, {"data", 3},
            {"info", 4}, {"debug", 5}, {"prompt", 6}, {"verbose", 7},
            {"input", 8}, {"silly", 9}
        },
        .colors = {
            {"error", "red"}, {"warn", "yellow"}, {"help", "cyan"},
            {"data", "grey"}, {"info", "green"}, {"debug", "blue"},
            {"prompt", "grey"}, {"verbose", "cyan"}, {"input", "grey"},
            {"silly", "magenta"}
        }
    };
}

inline bool LevelConfig::isLevelEnabled(const std::string& thresholdLevel,
                                         const std::string& messageLevel) const {
    auto thresholdIt = levels.find(thresholdLevel);
    auto messageIt = levels.find(messageLevel);
    if (thresholdIt == levels.end() || messageIt == levels.end()) {
        return false;
    }
    return thresholdIt->second >= messageIt->second;
}

inline int LevelConfig::maxLevel() const {
    if (levels.empty()) {
        return -1;
    }
    int max = -1;
    for (const auto& [name, priority] : levels) {
        if (priority > max) {
            max = priority;
        }
    }
    return max;
}

} // namespace winston
} // namespace polycpp
