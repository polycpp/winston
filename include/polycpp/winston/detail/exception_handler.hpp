#pragma once

/// @file detail/exception_handler.hpp
/// @brief Inline implementations for ExceptionHandler.

#include <polycpp/winston/exception_handler.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/core/date.hpp>
#include <polycpp/core/error.hpp>
#include <polycpp/core/json.hpp>
#include <polycpp/process.hpp>
#include <polycpp/os.hpp>

namespace polycpp {
namespace winston {

inline ExceptionHandler::ExceptionHandler(Logger& logger)
    : logger_(logger) {}

inline void ExceptionHandler::handle(
    const std::vector<std::shared_ptr<Transport>>& transports) {
    // Add any additional transports to the logger
    for (const auto& t : transports) {
        logger_.add(t);
    }

    // Install the process exception handler
    polycpp::process::on("uncaughtException",
        [this](const std::exception& err) {
            LogInfo info = getAllInfo(err);
            logger_.log(std::move(info));
        });

    installed_ = true;
}

inline void ExceptionHandler::unhandle() {
    if (installed_) {
        polycpp::process::removeAllListeners("uncaughtException");
        installed_ = false;
    }
}

inline LogInfo ExceptionHandler::getAllInfo(const std::exception& err) {
    LogInfo info;
    info.level = "error";
    info.originalLevel = "error";
    info.message = err.what();

    // Error details
    JsonObject errorObj;
    errorObj["message"] = JsonValue(std::string(err.what()));

    // Check if it's a polycpp::Error for richer info
    auto* pErr = dynamic_cast<const polycpp::Error*>(&err);
    if (pErr) {
        std::string stackStr = pErr->stack;
        if (!stackStr.empty()) {
            errorObj["stack"] = JsonValue(std::move(stackStr));
        }
    }
    info.metadata["error"] = JsonValue(std::move(errorObj));

    // Exception flag
    info.metadata["exception"] = JsonValue(true);

    // Date
    info.metadata["date"] = JsonValue(polycpp::Date().toISOString());

    // Process info
    info.metadata["process"] = JsonValue(getProcessInfo());

    // OS info
    info.metadata["os"] = JsonValue(getOsInfo());

    return info;
}

inline JsonObject ExceptionHandler::getProcessInfo() {
    JsonObject proc;
    proc["pid"] = JsonValue(static_cast<double>(polycpp::process::pid()));
    proc["cwd"] = JsonValue(polycpp::process::cwd());

    // Memory usage
    auto mem = polycpp::process::memoryUsage();
    proc["memoryUsage"] = mem;

    return proc;
}

inline JsonObject ExceptionHandler::getOsInfo() {
    JsonObject os;

    auto load = polycpp::os::loadavg();
    JsonArray la;
    for (auto v : load) {
        la.push_back(JsonValue(v));
    }
    os["loadavg"] = JsonValue(std::move(la));
    os["uptime"] = JsonValue(polycpp::os::uptime());

    return os;
}

} // namespace winston
} // namespace polycpp
