#pragma once

/// @file winston.hpp
/// @brief Main entry point for polycpp/winston logging library.

#include <string>

namespace polycpp {
namespace winston {

/// @brief Library version string.
/// @return Version in "major.minor.patch" format.
inline const std::string& version() {
    static const std::string v = "0.1.0";
    return v;
}

} // namespace winston
} // namespace polycpp
