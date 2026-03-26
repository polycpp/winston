#pragma once

/// @file transport.hpp
/// @brief Transport base class with level filtering and EventEmitter.

#include <polycpp/winston/log_info.hpp>
#include <polycpp/winston/format.hpp>
#include <polycpp/winston/level_config.hpp>
#include <polycpp/events.hpp>
#include <memory>
#include <optional>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Options for Transport construction.
struct TransportOptions {
    /// Per-transport format (applied after logger format, before log()).
    std::shared_ptr<Format> format;

    /// Per-transport level threshold. Empty string means inherit from logger.
    std::string level;

    /// Suppress this transport entirely.
    bool silent = false;

    /// Whether this transport handles uncaught exceptions.
    bool handleExceptions = false;

    /// Whether this transport handles unhandled rejections.
    bool handleRejections = false;
};

/// @brief Abstract base class for all transports.
///
/// Transports receive formatted LogInfo objects and write them to a
/// destination (console, file, network, etc.). Each transport can have
/// its own level threshold and format.
///
/// Extends polycpp::EventEmitter for events:
/// - "logged" -- emitted after a message is successfully written
/// - "error"  -- emitted when a write error occurs
/// - "warn"   -- emitted for non-fatal transport warnings
///
/// @see https://github.com/winstonjs/winston-transport
class Transport : public polycpp::EventEmitter {
public:
    virtual ~Transport() = default;

    /// @brief Write a log entry through this transport.
    ///
    /// Performs silent check, level filtering, applies per-transport format,
    /// then calls the virtual log() method. Emits "logged" on success.
    ///
    /// @param info The log entry to write.
    void write(LogInfo info);

    /// @brief Check if a message at the given level should be accepted.
    ///
    /// If this transport has its own level threshold, checks that the
    /// message level is severe enough. If no threshold is set, accepts all.
    ///
    /// @param info The log entry to check.
    /// @return True if the message should be logged by this transport.
    bool accept(const LogInfo& info) const;

    /// @brief Set the level configuration used for level filtering.
    ///
    /// Called by Logger when a transport is added. The transport stores
    /// a non-owning pointer to the logger's level config.
    ///
    /// @param levels Pointer to the level configuration.
    void setLevels(const LevelConfig* levels);

    /// @brief The transport's level threshold.
    std::string level;

    /// @brief Whether this transport is silent (suppresses all output).
    bool silent = false;

    /// @brief Whether this transport handles uncaught exceptions.
    bool handleExceptions = false;

    /// @brief Whether this transport handles unhandled rejections.
    bool handleRejections = false;

    /// @brief Per-transport format (applied after logger format).
    std::shared_ptr<Format> format;

protected:
    /// @brief Construct with options.
    /// @param options Transport configuration options.
    explicit Transport(TransportOptions options = {});

    /// @brief Write a log entry to the destination.
    ///
    /// Subclasses must implement this to perform the actual I/O.
    /// Called after level filtering and format application.
    ///
    /// @param info The fully formatted log entry.
    virtual void log(const LogInfo& info) = 0;

    /// Non-owning pointer to the logger's level config. Set by setLevels().
    const LevelConfig* levels_ = nullptr;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/transport.hpp>
