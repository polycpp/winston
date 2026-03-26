#pragma once

/// @file exception_handler.hpp
/// @brief ExceptionHandler class -- installs process-level uncaught exception logging.

#include <polycpp/winston/log_info.hpp>
#include <polycpp/winston/transport.hpp>
#include <polycpp/core/json.hpp>
#include <exception>
#include <memory>
#include <string>
#include <vector>

namespace polycpp {
namespace winston {

// Forward declaration
class Logger;

/// @brief Installs a process-level uncaught exception handler that logs
/// rich diagnostic information before the process exits.
///
/// Uses polycpp::process::on("uncaughtException", handler) to install
/// the handler. Gathers process and OS diagnostics including PID, CWD,
/// memory usage, load averages, and uptime.
///
/// @par Example
/// ```cpp
/// Logger logger(LoggerOptions{
///     .level = "error",
///     .transports = { std::make_shared<ConsoleTransport>() }
/// });
/// ExceptionHandler handler(logger);
/// handler.handle();
/// // Uncaught exceptions will now be logged with diagnostics
/// ```
///
/// @see https://github.com/winstonjs/winston/blob/master/lib/winston/exception-handler.js
class ExceptionHandler {
public:
    /// @brief Construct an exception handler bound to a logger.
    /// @param logger Reference to the Logger to log exceptions to.
    explicit ExceptionHandler(Logger& logger);

    /// @brief Install the uncaught exception handler.
    ///
    /// Optionally adds extra transports to the logger before installing.
    ///
    /// @param transports Additional transports to add to the logger.
    void handle(const std::vector<std::shared_ptr<Transport>>& transports = {});

    /// @brief Remove the exception handler.
    void unhandle();

    /// @brief Build a rich diagnostic LogInfo from an exception.
    ///
    /// Produces a LogInfo with level "error", the exception message, and
    /// metadata containing error details, process info, OS info, and a
    /// timestamp.
    ///
    /// @param err The exception to build diagnostics for.
    /// @return A LogInfo populated with diagnostic information.
    static LogInfo getAllInfo(const std::exception& err);

    /// @brief Get process diagnostics as a JsonObject.
    ///
    /// Includes PID, CWD, and memory usage.
    ///
    /// @return JsonObject with process diagnostic fields.
    static JsonObject getProcessInfo();

    /// @brief Get OS diagnostics as a JsonObject.
    ///
    /// Includes load averages and uptime.
    ///
    /// @return JsonObject with OS diagnostic fields.
    static JsonObject getOsInfo();

private:
    Logger& logger_;
    bool installed_ = false;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/exception_handler.hpp>
