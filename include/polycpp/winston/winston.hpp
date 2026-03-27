#pragma once

/// @file winston.hpp
/// @brief Main entry point for polycpp/winston logging library.
///
/// Provides createLogger() factory, default logger singleton, namespace-level
/// convenience functions, and the default Container instance. This header
/// re-exports all public types for convenience.
///
/// @see https://github.com/winstonjs/winston

// Re-export all public headers
#include <polycpp/winston/log_info.hpp>
#include <polycpp/winston/level_config.hpp>
#include <polycpp/winston/format.hpp>
#include <polycpp/winston/colorizer.hpp>
#include <polycpp/winston/transport.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/winston/profiler.hpp>
#include <polycpp/winston/child_logger.hpp>
#include <polycpp/winston/console_transport.hpp>
#include <polycpp/winston/stream_transport.hpp>
#include <polycpp/winston/file_transport.hpp>
#include <polycpp/winston/container.hpp>
#include <polycpp/winston/exception_handler.hpp>

#include <map>
#include <memory>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Library version string.
/// @return Version in "major.minor.patch" format.
inline const std::string& version() {
    static const std::string v = "0.1.0";
    return v;
}

/// @brief Create a new Logger with the given options.
///
/// Factory function matching winston's `createLogger()`.
///
/// @param options Configuration options for the new logger.
/// @return Shared pointer to the new Logger.
///
/// @par Example
/// @code
/// auto logger = winston::createLogger({
///     .level = "info",
///     .transports = { std::make_shared<ConsoleTransport>() }
/// });
/// logger->info("Hello, world!");
/// @endcode
///
/// @see https://github.com/winstonjs/winston#creating-your-own-logger
std::shared_ptr<Logger> createLogger(LoggerOptions options = {});

/// @brief Get the default logger singleton.
///
/// Created on first access with default options (level="info", npm levels,
/// no format, no transports). Users must call winston::add(transport)
/// or winston::configure() before logging produces visible output.
///
/// @return Reference to the default Logger.
/// @see https://github.com/winstonjs/winston#default-logger
Logger& defaultLogger();

/// @brief Configure the default logger with new options.
///
/// Replaces the default logger's configuration entirely.
///
/// @param opts New configuration options.
void configure(const LoggerOptions& opts);

// --- Default logger pass-through functions ---

/// @brief Log at a specified level on the default logger.
/// @param level The log level name.
/// @param message The log message.
/// @return Reference to the default logger for chaining.
Logger& log(const std::string& level, const std::string& message);

/// @brief Log at a specified level with metadata on the default logger.
/// @param level The log level name.
/// @param message The log message.
/// @param meta Additional metadata.
/// @return Reference to the default logger for chaining.
Logger& log(const std::string& level, const std::string& message,
            const JsonObject& meta);

/// @brief Log a pre-built LogInfo on the default logger.
/// @param info The log entry.
/// @return Reference to the default logger for chaining.
Logger& log(LogInfo info);

/// @brief Log at error level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& error(const std::string& message, const JsonObject& meta = {});

/// @brief Log at warn level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& warn(const std::string& message, const JsonObject& meta = {});

/// @brief Log at info level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& info(const std::string& message, const JsonObject& meta = {});

/// @brief Log at http level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& http(const std::string& message, const JsonObject& meta = {});

/// @brief Log at verbose level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& verbose(const std::string& message, const JsonObject& meta = {});

/// @brief Log at debug level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& debug(const std::string& message, const JsonObject& meta = {});

/// @brief Log at silly level on the default logger.
/// @param message The log message.
/// @param meta Optional metadata.
/// @return Reference to the default logger for chaining.
Logger& silly(const std::string& message, const JsonObject& meta = {});

/// @brief Add a transport to the default logger.
/// @param transport The transport to add.
/// @return Reference to the default logger for chaining.
Logger& add(std::shared_ptr<Transport> transport);

/// @brief Remove a transport from the default logger.
/// @param transport The transport to remove.
/// @return Reference to the default logger for chaining.
Logger& remove(std::shared_ptr<Transport> transport);

/// @brief Remove all transports from the default logger.
/// @return Reference to the default logger for chaining.
Logger& clear();

/// @brief Toggle-based profiling on the default logger.
/// @param id The profiler identifier.
/// @see Logger::profile
void profile(const std::string& id);

/// @brief Create a Profiler on the default logger that starts timing immediately.
/// @return A Profiler bound to the default logger.
/// @see Logger::startTimer
Profiler startTimer();

/// @brief Create a child logger from the default logger.
/// @param defaultMeta Default metadata for the child.
/// @return A ChildLogger bound to the default logger.
/// @see Logger::child
ChildLogger child(const JsonObject& defaultMeta);

/// @brief Get the default named-logger container singleton.
///
/// @return Reference to the default Container.
/// @see https://github.com/winstonjs/winston#working-with-multiple-loggers-in-winston
Container& loggers();

/// @brief Register custom level colors globally.
///
/// Delegates to the Colorizer's static addColors method.
///
/// @param colors Map of level name -> space-separated color/style names.
/// @see https://github.com/winstonjs/winston#using-custom-logging-levels
void addColors(const std::map<std::string, std::string>& colors);

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/winston.hpp>
