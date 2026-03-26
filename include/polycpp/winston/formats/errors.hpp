#pragma once

/// @file formats/errors.hpp
/// @brief Errors format -- extracts error properties into metadata.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the errors format.
struct ErrorsOptions {
    /// If true, include the stack trace in metadata.
    bool stack = true;
};

/// @brief Extracts error properties into log entry metadata.
///
/// Checks if `metadata["error"]` contains a serialized error object
/// (with "message", "stack", "name" fields) and extracts those fields
/// into the top-level metadata. If `info.message` is empty, copies the
/// error's message into it.
///
/// @see https://github.com/winstonjs/logform#errors
class ErrorsFormat : public Format {
public:
    /// @brief Construct an errors format.
    /// @param options Errors options.
    explicit ErrorsFormat(ErrorsOptions options = {});

    /// @brief Extract error fields from metadata.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with error fields extracted.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    ErrorsOptions opts_;
};

/// @brief Factory function for errors format.
/// @param options Errors options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = errors({.stack = true});
/// ```
///
/// @see https://github.com/winstonjs/logform#errors
std::shared_ptr<Format> errors(ErrorsOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/errors.hpp>
