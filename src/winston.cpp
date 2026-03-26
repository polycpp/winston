/// @file winston.cpp
/// @brief Compiled translation unit for polycpp/winston.

#include <polycpp/winston/winston.hpp>

// Placeholder -- static data (Colorizer registry, etc.) will live here.
// Needed so the library has at least one compiled TU for the linker.
namespace polycpp {
namespace winston {
namespace detail {

// Force at least one symbol into the compiled library.
static const int kLibraryInitialized = 1;

} // namespace detail
} // namespace winston
} // namespace polycpp
