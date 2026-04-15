#pragma once

/// @file events.hpp
/// @brief Typed event constants for polycpp/winston.

#include <polycpp/events/typed_event.hpp>
#include <polycpp/core/error.hpp>
#include <string>

namespace polycpp {
namespace winston {
namespace event {

/// @brief Transport/logger error event. Callback: `void(const Error&)`.
constexpr events::TypedEvent<"error", const Error&> Error_;

/// @brief Non-fatal warning event. Callback: `void(const std::string&)`.
constexpr events::TypedEvent<"warn", const std::string&> Warn;

/// @brief Transport logged event. Callback: `void()`.
constexpr events::TypedEvent<"logged"> Logged;

/// @brief Logger close event. Callback: `void()`.
constexpr events::TypedEvent<"close"> Close;

} // namespace event
} // namespace winston
} // namespace polycpp
