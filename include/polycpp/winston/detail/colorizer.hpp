#pragma once

/// @file detail/colorizer.hpp
/// @brief Inline implementations for Colorizer.

#include <polycpp/winston/colorizer.hpp>
#include <polycpp/util.hpp>
#include <sstream>
#include <vector>

namespace polycpp {
namespace winston {

inline void Colorizer::addColors(const std::map<std::string, std::string>& colors) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [level, color] : colors) {
        allColors_[level] = color;
    }
}

inline std::map<std::string, std::string> Colorizer::allColors() {
    std::lock_guard<std::mutex> lock(mutex_);
    return allColors_;
}

inline std::string Colorizer::colorize(const std::string& level,
                                       const std::string& text) {
    std::string colorStr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allColors_.find(level);
        if (it == allColors_.end()) {
            return text; // No color registered for this level
        }
        colorStr = it->second;
    }

    // Split space-separated styles and apply each one (nested).
    // e.g., "bold red" -> styleText("red", styleText("bold", text))
    // Apply left-to-right: first style is innermost.
    std::vector<std::string> styles;
    std::istringstream iss(colorStr);
    std::string style;
    while (iss >> style) {
        styles.push_back(style);
    }

    std::string result = text;
    for (const auto& s : styles) {
        result = polycpp::util::styleText(s, result);
    }
    return result;
}

inline std::string Colorizer::strip(const std::string& str) {
    return polycpp::util::stripVTControlCharacters(str);
}

} // namespace winston
} // namespace polycpp
