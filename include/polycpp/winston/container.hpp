#pragma once

/// @file container.hpp
/// @brief Container class -- named logger registry for sharing loggers across modules.

#include <polycpp/winston/logger.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Named logger registry that manages shared Logger instances.
///
/// Enables sharing loggers across modules: `container.get("api")` returns
/// the same instance every time. When a logger's "close" event fires, it
/// is automatically removed from the container.
///
/// @par Example
/// ```cpp
/// Container container;
/// auto api = container.get("api", {.level = "debug"});
/// auto web = container.get("web");
/// assert(container.has("api"));
/// container.close();
/// ```
///
/// @see https://github.com/winstonjs/winston/blob/master/lib/winston/container.js
class Container {
public:
    /// @brief Construct with default options applied to loggers created by get().
    /// @param defaults Default LoggerOptions for new loggers.
    explicit Container(LoggerOptions defaults = {});

    // Non-copyable, non-movable (captured 'this' in callbacks would be invalidated)
    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;

    /// @brief Get or create a named logger.
    ///
    /// If the logger already exists, returns it (ignoring opts).
    /// If it does not exist, creates it with opts (falling back to defaults
    /// for fields not explicitly set).
    ///
    /// @param id The logger identifier.
    /// @param opts Options for the new logger (ignored if logger exists).
    /// @return Shared pointer to the logger.
    /// @see https://github.com/winstonjs/winston#working-with-multiple-loggers-in-winston
    std::shared_ptr<Logger> get(const std::string& id,
                                 const LoggerOptions& opts = {});

    /// @brief Alias for get().
    ///
    /// @param id The logger identifier.
    /// @param opts Options for the new logger.
    /// @return Shared pointer to the logger.
    std::shared_ptr<Logger> add(const std::string& id,
                                 const LoggerOptions& opts = {});

    /// @brief Check if a named logger exists in the container.
    ///
    /// @param id The logger identifier.
    /// @return True if the logger exists.
    bool has(const std::string& id) const;

    /// @brief Close a specific named logger and remove it from the container.
    ///
    /// If the logger does not exist, this is a no-op.
    ///
    /// @param id The logger identifier.
    void close(const std::string& id);

    /// @brief Close all loggers and clear the container.
    void close();

private:
    LoggerOptions defaults_;
    std::map<std::string, std::shared_ptr<Logger>> loggers_;
    mutable std::mutex mutex_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/container.hpp>
