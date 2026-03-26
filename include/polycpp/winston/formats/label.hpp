#pragma once

/// @file formats/label.hpp
/// @brief Label format -- adds a label to log entry metadata or message.

#include <polycpp/winston/format.hpp>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the label format.
struct LabelOptions {
    /// The label string to add.
    std::string label;

    /// If true, prepend "[label] " to message instead of adding to metadata.
    bool message = false;
};

/// @brief Adds a label to each log entry.
///
/// By default, sets `metadata["label"]` to the configured label string.
/// If `message` option is true, prepends `[label] ` to the message instead.
///
/// @see https://github.com/winstonjs/logform#label
class LabelFormat : public Format {
public:
    /// @brief Construct a label format.
    /// @param options Label options.
    explicit LabelFormat(LabelOptions options = {});

    /// @brief Add label to the log entry.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with label added.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    LabelOptions opts_;
};

/// @brief Factory function for label format.
/// @param options Label options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = label({.label = "my-app"});
/// auto fmt = label({.label = "my-app", .message = true});
/// ```
///
/// @see https://github.com/winstonjs/logform#label
std::shared_ptr<Format> label(LabelOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/label.hpp>
