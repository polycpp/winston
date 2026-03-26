#pragma once

/// @file format.hpp
/// @brief Format base class, CombinedFormat, and format composition utilities.

#include <polycpp/winston/log_info.hpp>
#include <polycpp/core/json.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace polycpp {
namespace winston {

/// @brief Options passed to Format constructors.
///
/// A thin wrapper around JsonObject for maximum flexibility, matching
/// how JS formats receive arbitrary option objects.
using FormatOptions = JsonObject;

/// @brief Abstract base class for all log formats.
///
/// A Format transforms a LogInfo object, typically mutating it in place.
/// Returning std::nullopt filters (drops) the message.
///
/// Maps to logform's Format class: `format((info, opts) => ...)`.
///
/// @see https://github.com/winstonjs/logform
class Format {
public:
    virtual ~Format() = default;

    /// @brief Transform a log entry.
    ///
    /// Formats mutate the LogInfo in place (enrichment, mutation, finalization).
    /// Return std::nullopt to filter/drop the message.
    ///
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo, or std::nullopt to drop.
    virtual std::optional<LogInfo> transform(LogInfo info) = 0;

protected:
    /// Options passed at format construction time.
    FormatOptions options_;

    /// @brief Construct with options.
    /// @param options Format-specific options.
    explicit Format(FormatOptions options = {});
};

/// @brief Chains multiple formats sequentially.
///
/// Implements winston's `format.combine(f1, f2, f3)`. Each format's
/// output becomes the next format's input. If any format returns
/// std::nullopt, the entire chain short-circuits and returns std::nullopt.
///
/// @see https://github.com/winstonjs/logform#combine
class CombinedFormat : public Format {
public:
    /// @brief Construct a combined format from a list of formats.
    /// @param formats The formats to chain, applied in order.
    explicit CombinedFormat(std::vector<std::shared_ptr<Format>> formats);

    /// @brief Apply all formats sequentially.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo, or std::nullopt if any format filters.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    std::vector<std::shared_ptr<Format>> formats_;
};

/// @brief Create a combined format that chains multiple formats.
///
/// Convenience factory matching winston's `format.combine(...)`.
///
/// @param formats The formats to chain.
/// @return A shared_ptr to the combined format.
///
/// @par Example
/// ```cpp
/// auto fmt = combine({timestamp(), colorize(), simple()});
/// ```
std::shared_ptr<Format> combine(std::vector<std::shared_ptr<Format>> formats);

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/format.hpp>
