/// @file winston.cpp
/// @brief Compiled translation unit for polycpp/winston.
///
/// Contains static data that cannot be in header-only inline code:
/// - Colorizer global color registry and mutex

#include <polycpp/winston/winston.hpp>
#include <polycpp/winston/colorizer.hpp>

namespace polycpp {
namespace winston {

// Colorizer static members
std::map<std::string, std::string> Colorizer::allColors_;
std::mutex Colorizer::mutex_;

} // namespace winston
} // namespace polycpp
